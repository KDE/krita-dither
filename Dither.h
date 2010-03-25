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

#ifndef _DITHER_H_
#define _DITHER_H_

// TODO: remove that
#define LCMS_HEADER <lcms.h>
// TODO: remove it !

#include <kparts/plugin.h>
#include <kis_filter.h>

class DitherFilterConfig;

class KritaDither : public QObject
{
public:
    KritaDither(QObject *parent, const QVariantList &);
    virtual ~KritaDither();
};

class KisDitherFilter : public KisFilter
{
public:
    KisDitherFilter();
public:
    using KisFilter::process;
    virtual void process(KisConstProcessingInformation src,
                         KisProcessingInformation dst,
                         const QSize& size,
                         const KisFilterConfiguration* config,
                         KoUpdater* progressUpdater
                        ) const;
    virtual ColorSpaceIndependence colorSpaceIndependence() { return FULLY_INDEPENDENT; };
    static inline KoID id() { return KoID("dither", i18n("Dither")); };
    virtual bool supportsPainting() { return false; }
    virtual bool supportsPreview() { return false; }
    virtual bool supportsIncrementalPainting() { return false; }
    virtual bool supportsAdjustmentLayers() { return true; }
    virtual KisConfigWidget * createConfigurationWidget(QWidget * parent, const KisPaintDeviceSP dev, const KisImageWSP image = 0) const;
    virtual KisFilterConfiguration* configuration();
private:
    std::vector<QColor> optimizeColors( const std::map<QColor, int>& colors2int, int paletteSize, int& pixelsProcessed, KoUpdater* progressUpdater ) const;
    void generateOptimizedPalette(quint8** colorPalette, int reduction, KisPaintDeviceSP src, const QRect& rect, int paletteSize, int& pixelsProcessed, KoUpdater* progressUpdater ) const;
};

#endif
