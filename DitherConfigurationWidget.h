/*
 * This file is part of Krita
 *
 * Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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

#ifndef KIS_DITHER_CONFIGURATION_WIDGET_H_
#define KIS_DITHER_CONFIGURATION_WIDGET_H_

// TODO: remove that
#define LCMS_HEADER <lcms.h>
// TODO: remove it !

#include <kis_config_widget.h>

class KisFilter;
class Ui_DitherConfigurationBaseWidget;
class QListViewItem;

/**
        @author Cyrille Berger <cberger@cberger.net>
*/
class DitherConfigurationWidget : public KisConfigWidget
{
    Q_OBJECT
    public:
        DitherConfigurationWidget(QWidget* parent);
        ~DitherConfigurationWidget();
        virtual void setConfiguration(const KisPropertiesConfiguration * config);
        virtual KisPropertiesConfiguration* configuration() const;
    private:
        Ui_DitherConfigurationBaseWidget* m_widget;
};

#endif
