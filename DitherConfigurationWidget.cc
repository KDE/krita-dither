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

#include "DitherConfigurationWidget.h"

#include <qlayout.h>
#include <qcombobox.h>
#include <klistview.h>
#include <knuminput.h>
#include <klocale.h>

#include "DitherConfigurationBaseWidget.h"

DitherConfigurationWidget::DitherConfigurationWidget(QWidget * parent, const char * name) : KisFilterConfigWidget ( parent, name )
{
    QGridLayout *widgetLayout = new QGridLayout(this, 1, 1);
    m_widget = new DitherConfigurationBaseWidget(this);
    widgetLayout -> addWidget(m_widget,0,0);
    connect(m_widget->paletteType, SIGNAL(currentChanged(QListViewItem*)), SLOT(sigPleaseUpdatePreview()));
    connect(m_widget->paletteSize, SIGNAL(valueChanged(int)), SIGNAL(sigPleaseUpdatePreview()));
}


DitherConfigurationWidget::~DitherConfigurationWidget()
{
}

void DitherConfigurationWidget::setConfiguration(KisFilterConfiguration* config)
{
    QVariant value;
    if (config->getProperty("paletteSize", value))
    {
        widget()->paletteSize->setValue(value.toInt(0));
    }
    if (config->getProperty("paletteType", value))
    {
        widget()->paletteType->setCurrentItem(value.toInt(0));
    }
}

#include "DitherConfigurationWidget.moc"
