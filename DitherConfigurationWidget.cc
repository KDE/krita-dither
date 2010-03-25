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
#include <knuminput.h>
#include <klocale.h>
#include <kis_filter_configuration.h>

#include "ui_DitherConfigurationBaseWidget.h"
#include "Dither.h"

DitherConfigurationWidget::DitherConfigurationWidget(QWidget * parent) : KisConfigWidget ( parent )
{
    m_widget = new Ui_DitherConfigurationBaseWidget;
    m_widget->setupUi(this);
    connect(m_widget->paletteType, SIGNAL(activated(int)), SIGNAL(sigPleaseUpdatePreview()));
    connect(m_widget->paletteSize, SIGNAL(valueChanged(int)), SIGNAL(sigPleaseUpdatePreview()));
}


DitherConfigurationWidget::~DitherConfigurationWidget()
{
}

void DitherConfigurationWidget::setConfiguration(const KisPropertiesConfiguration* config)
{
    QVariant value;
    if (config->getProperty("paletteSize", value))
    {
        m_widget->paletteSize->setValue(value.toInt(0));
    }
    if (config->getProperty("paletteType", value))
    {
        m_widget->paletteType->setCurrentIndex(value.toInt(0));
    }
}

KisPropertiesConfiguration* DitherConfigurationWidget::configuration() const
{
    KisFilterConfiguration* config = new KisFilterConfiguration(KisDitherFilter::id().id(),1);
    config->setProperty("paletteSize", m_widget->paletteSize->value() );
    config->setProperty("paletteType", m_widget->paletteType->currentIndex() );
    return config;
}

#include "DitherConfigurationWidget.moc"
