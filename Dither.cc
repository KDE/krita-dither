/*
 * This file is part of the KDE project
 *
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "Dither.h"

#include <stdlib.h>
#include <vector>

#include <klocale.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kdebug.h>
#include <kgenericfactory.h>

#include <kis_multi_double_filter_widget.h>
#include <kis_iterators_pixel.h>
#include <kis_progress_display_interface.h>
#include <kis_filter_registry.h>
#include <kis_global.h>
#include <kis_transaction.h>
#include <kis_types.h>
#include <kis_selection.h>

#include <kis_convolution_painter.h>

#include <qimage.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qcombobox.h>

#include "DitherConfigurationWidget.h"
#include "DitherConfigurationBaseWidget.h"

typedef Q_UINT8 quint8;

typedef KGenericFactory<KritaDither> KritaDitherFactory;
K_EXPORT_COMPONENT_FACTORY( kritaDither, KritaDitherFactory( "krita" ) )

KritaDither::KritaDither(QObject *parent, const char *name, const QStringList &)
: KParts::Plugin(parent, name)
{
    setInstance(KritaDitherFactory::instance());

    kdDebug(41006) << "Dither filter plugin. Class: "
    << className()
    << ", Parent: "
    << parent -> className()
    << "\n";

    if(parent->inherits("KisFilterRegistry"))
    {
        KisFilterRegistry * manager = dynamic_cast<KisFilterRegistry *>(parent);
        manager->add(new KisDitherFilter());
    }
}

KritaDither::~KritaDither()
{
}

KisDitherFilter::KisDitherFilter() 
    : KisFilter(id(), "dither", i18n("&Dither"))
{
}

KisFilterConfiguration* KisDitherFilter::configuration()
{
    KisFilterConfiguration* config = new KisFilterConfiguration(id().id(),1);
    config->setProperty("paletteSize", 16);
    config->setProperty("paletteType", 0);
    return config;
};

KisFilterConfigWidget * KisDitherFilter::createConfigurationWidget(QWidget* parent, KisPaintDeviceSP /*dev*/)
{
    DitherConfigurationWidget* w = new DitherConfigurationWidget(parent, "");
    Q_CHECK_PTR(w);
    return w;
}

KisFilterConfiguration* KisDitherFilter::configuration(QWidget* nwidget)
{
    DitherConfigurationWidget* widget = (DitherConfigurationWidget*) nwidget;
    if( widget == 0 )
    {
        return configuration();
    } else {
        DitherConfigurationBaseWidget* baseWidget = widget->widget();
        KisFilterConfiguration* config = new KisFilterConfiguration(id().id(),1);
        config->setProperty("paletteSize", baseWidget->paletteSize->value() );
        config->setProperty("paletteType", baseWidget->paletteType->currentItem() );
        return config;
    }
}

bool operator<(const QColor& c1, const QColor& c2)
{
    if(c1.red() < c2.red()) return true;
    else if(c1.red() > c2.red()) return false;
    if(c1.green() < c2.green()) return true;
    else if(c1.green() > c2.green()) return false;
    if(c1.blue() < c2.blue()) return true;
    else if(c1.blue() > c2.blue()) return false;
    return false;
}

void KisDitherFilter::process(KisPaintDeviceSP src, KisPaintDeviceSP dst, 
                                   KisFilterConfiguration* config, const QRect& rect ) 
{
    Q_ASSERT(src != 0);
    Q_ASSERT(dst != 0);
    
    // Dither analysis
    KisColorSpace * cs = src->colorSpace();
    Q_INT32 pixelSize = cs->pixelSize();
    
    QVariant value;
    int paletteSize = 16;
    if (config->getProperty("paletteSize", value))
    {
        paletteSize = value.toInt(0);
    }
    int paletteType = 0;
    if (config->getProperty("paletteType", value))
    {
        paletteType = value.toInt(0);
    }
    quint8** colorPalette = new quint8*[paletteSize];
    switch(paletteType)
    {
        case 1:
        {
            QColor c;
            std::map<QColor, int> colors2int;
            KisRectIteratorPixel rectIt = src->createRectIterator(rect.x(), rect.y(), rect.width(), rect.height(), false);
            while(not rectIt.isDone())
            {
                cs->toQColor( rectIt.oldRawData(), &c, (KisProfile*)0 );
                colors2int[ c ] += 1;
                ++rectIt;
            }
            std::multimap<int, QColor> int2colors;
            for( std::map<QColor, int>::iterator it = colors2int.begin();
                 it != colors2int.end(); ++it)
            {
                int2colors.insert( std::multimap<int, QColor>::value_type(-it->second, it->first) );
            }
            int realPaletteSize = 0;
            for( std::multimap<int , QColor>::iterator it = int2colors.begin();
                 it != int2colors.end() and realPaletteSize < paletteSize; ++it, ++realPaletteSize)
            {
                quint8* color = new quint8[ pixelSize ];
                cs->fromQColor( it->second, color, 0 );
                colorPalette[realPaletteSize] = color;
            }
            paletteSize = realPaletteSize;
            break;
        }
        default:
        case 0:
            for(int i = 0; i < paletteSize; i++)
            {
                QColor c( (int)(rand() * 255.0 / RAND_MAX), (int)(rand() * 255.0 / RAND_MAX), (int)(rand() * 255.0 / RAND_MAX) );
                quint8* color = new quint8[ pixelSize ];
                cs->fromQColor( c, color, 0 );
                colorPalette[i] = color;
            }
            break;
    }
    
    // Apply palette
    KisHLineIteratorPixel dstIt = dst->createHLineIterator(rect.x(), rect.y(), rect.width(), true );
    KisHLineIteratorPixel srcIt = src->createHLineIterator(rect.x(), rect.y(), rect.width(), false);
    int pixelsProcessed = 0;
    setProgressTotalSteps(rect.width() * rect.height());
    
    for(int y = 0; y < rect.height(); y++)
    {
        while( not srcIt.isDone() )
        {
            if(srcIt.isSelected())
            {
                quint8* bestColor = 0;
                double bestDifference = 0.0;
                const quint8* rawData = srcIt.oldRawData();
                for(int i = 0; i < paletteSize; i++)
                {
                    quint8* color = colorPalette[i];
#if 0
                    double delta = cs->difference( color, rawData );
#endif
#if 1
                    double delta = 0.0;
                    for(int j = 0; j < pixelSize; j++)
                    {
                        double a = (color[j] - rawData[j]);
                        delta += a * a;
                    }
#endif
                    if(delta < bestDifference or not bestColor)
                    {
                        bestDifference = delta;
                        bestColor = color;
                    }
                }
                Q_ASSERT(bestColor);
                memcpy( dstIt.rawData(), bestColor, pixelSize);
            }
            setProgress(++pixelsProcessed);
            ++srcIt;
            ++dstIt;
        }
        srcIt.nextRow();
        dstIt.nextRow();
    }

    // Delete palette
    for(int i = 0; i < paletteSize; i++)
    {
      delete[] colorPalette[i];
    }
    delete[] colorPalette;
    setProgressDone(); // Must be called even if you don't really support progression
}
