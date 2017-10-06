/****************************************************************************
**
** Copyright (C) 2016 Lorn Potter <lorn.potter@gmail.com7
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "window.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QProcess>
#include <QAction>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QMenu>
#include <QPushButton>
#include <QMessageBox>
#include <QSensorManager>
#include <QOrientationReading>
#include <QOrientationSensor>
#include <QDebug>

Window::Window()
{
    QSensorManager::setDefaultBackend(QByteArray(QOrientationSensor::type),
                                      QByteArray("iio-sensor-proxy.orientationsensor"));
    if (!orientationSensor.connectToBackend())
        qWarning() << "Could not connect to iio-sensor-proxy backend";

    connect(&orientationSensor, &QOrientationSensor::readingChanged,
            this, &Window::orientationReadingChanged);

    createActions();
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);
    hide();
    trayIcon->show();

    orientationSensor.start();
    if (!orientationSensor.isActive()) {
        qWarning("Orientationsensor didn't start!");

    }
}

void Window::orientationReadingChanged()
{
    qDebug() << Q_FUNC_INFO << orientationSensor.reading()->orientation();
    QString output;
    // normal', 'left', 'right' or 'inverted'

    switch (orientationSensor.reading()->orientation()) {
        case QOrientationReading::TopUp:
        case QOrientationReading::FaceUp:
        case QOrientationReading::FaceDown:
            {
                output = "normal";
            }
        break;
        case QOrientationReading::TopDown:
        output = "inverted";
        break;
        case QOrientationReading::LeftUp:
        output = "left";
        break;
        case QOrientationReading::RightUp:
        output = "right";
        break;
        default:
        break;
    };
    if (!output.isEmpty())
        runProcess(output);
}

void Window::runProcess(const QString &xrandrString)
{
    qDebug() << Q_FUNC_INFO << xrandrString;
    // xrandr --output LVDS1 --rotate < >

    QString program = "/usr/bin/xrandr";
    QStringList arguments;
    arguments << "-o" << xrandrString;

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

void Window::setVisible(bool visible)
{
    QDialog::setVisible(visible);
}

void Window::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {

        hide();
        event->ignore();
    }
}

void Window::setIcon()
{
    QIcon icon = QIcon(":/images/tray.svg");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << Q_FUNC_INFO;
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}

void Window::showMessage()
{
//    showIconCheckBox->setChecked(true);
//    QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(
//            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
//    if (msgIcon == QSystemTrayIcon::NoIcon) {
//        QIcon icon(iconComboBox->itemIcon(iconComboBox->currentIndex()));
//        trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
//                          durationSpinBox->value() * 1000);
//    } else {
//        trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), msgIcon,
//                          durationSpinBox->value() * 1000);
//    }
}

void Window::createActions()
{
    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void Window::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    setIcon();
    trayIcon->setContextMenu(trayIconMenu);
}

#endif
