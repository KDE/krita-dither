/*
 * This file is part of the KDE project
 *
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
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

void KisDitherFilter::process(KisPaintDeviceSP src, KisPaintDeviceSP dst, 
                                   KisFilterConfiguration* /*config*/, const QRect& rect ) 
{
    Q_ASSERT(src != 0);
    Q_ASSERT(dst != 0);
    
    // Dither analysis
    KisColorSpace * cs = src->colorSpace();
    Q_INT32 pixelSize = cs->pixelSize();
    
#if 0
    QColor c;
    QMap<QColor, int> m_values;
    {
      KisRectIteratorPixel rectIt = src->createRectIterator(rect.x(), rect.y(), rect.width(), rect.height(), false);
      while(not rectIt.isDone())
      {
        
        ++rectIt;
      }
    }
#endif
    QValueList<QColor> qColorPalette;
    for(int i = 0; i < 255; i++)
    {
        QColor c( (int)(rand() * 255.0 / RAND_MAX), (int)(rand() * 255.0 / RAND_MAX), (int)(rand() * 255.0 / RAND_MAX) );
        qColorPalette.append( c );
    }
    
    
    // Apply palette
    int paletteSize = qColorPalette.size();
    quint8** colorPalette = new quint8*[paletteSize];
    for(int i = 0; i < paletteSize; i++)
    {
      quint8* color = new quint8[ pixelSize ];
      cs->fromQColor( qColorPalette[i], color, 0 );
      colorPalette[i] = color;
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
