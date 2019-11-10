/*
    Copyright 2019 Benjamin Vedder	benjamin@vedder.se

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QColor>
#include <QImage>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mOverlayUpdating = false;

    QImage img(128, 128, QImage::Format_ARGB32);
    QPainter p(&img);

    p.fillRect(0, 0, 128, 128, Qt::darkRed);
    p.setBrush(Qt::black);
    p.drawEllipse(0, 0, 128, 128);
    ui->fullEditor->getEdit()->loadMaskImage(img);

    on_updateSizeButton_clicked();

    connect(ui->ovCrHBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovCrWBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovCrXPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovCrYPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->overlayBox, SIGNAL(toggled(bool)), this, SLOT(updateOverlay()));
    connect(ui->ovImCXPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovImCYPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovRotBox, SIGNAL(valueChanged(double)), this, SLOT(updateOverlay()));
    connect(ui->ovScaleBox, SIGNAL(valueChanged(double)), this, SLOT(updateOverlay()));
    connect(ui->ovRXPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovRYPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovXPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovYPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->ovTrBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));

    connect(ui->fontOverlayBox, SIGNAL(toggled(bool)), this, SLOT(updateOverlay()));
    connect(ui->fontBoldBox, SIGNAL(toggled(bool)), this, SLOT(updateOverlay()));
    connect(ui->fontBorderBox, SIGNAL(toggled(bool)), this, SLOT(updateOverlay()));
    connect(ui->fontAABox, SIGNAL(toggled(bool)), this, SLOT(updateOverlay()));
    connect(ui->fontXPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->fontYPosBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->fontWBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->fontHBox, SIGNAL(valueChanged(int)), this, SLOT(updateOverlay()));
    connect(ui->fontBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateOverlay()));
    connect(ui->fontSampleEdit, SIGNAL(textChanged(QString)), this, SLOT(updateOverlay()));
    connect(ui->fontScaleBox, SIGNAL(valueChanged(double)), this, SLOT(updateOverlay()));

    updateOverlay();

    connect(ui->fullEditor->getEdit(), &DisplayEdit::imageUpdated, [this](QImage image) {
        (void)image;

        if (!mOverlayUpdating) {
            updateOverlay();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_updateSizeButton_clicked()
{
    ui->customEditor->updateSize(ui->wBox->value(), ui->hBox->value());
}

void MainWindow::on_ovSaveButton_clicked()
{
    ui->fullEditor->getEdit()->saveOverlayToLayer2();
}

void MainWindow::updateOverlay()
{
    mOverlayUpdating = true;

    ui->fullEditor->getEdit()->clearOverlayImage();

    if (ui->overlayBox->isChecked()) {
        QColor transparent = ui->fullEditor->paletteColor(ui->ovTrBox->value());

        if (ui->ovTrBox->value() < 0) {
            transparent = Qt::red;
        }

        ui->fullEditor->getEdit()->setOverlayImage(
                    ui->ovXPosBox->value(), ui->ovYPosBox->value(),
                    ui->ovCrXPosBox->value(), ui->ovCrWBox->value(),
                    ui->ovCrYPosBox->value(), ui->ovCrHBox->value(),
                    ui->ovImCXPosBox->value(), ui->ovImCYPosBox->value(),
                    ui->ovRXPosBox->value(), ui->ovRYPosBox->value(),
                    ui->ovRotBox->value(),
                    ui->ovScaleBox->value(),
                    transparent,
                    ui->customEditor->getEdit()->getImageNow());
    }

    QString str = ui->fontSampleEdit->text();
    if (ui->fontOverlayBox->isChecked() && str.size() > 0) {
        int w = ui->fontWBox->value();
        int h = ui->fontHBox->value();
        int xPos = ui->fontXPosBox->value();
        int yPos = ui->fontYPosBox->value();
        QImage img(w * str.size(), h, QImage::Format_ARGB32);
        QPainter p(&img);

        p.drawImage(img.rect(), ui->fullEditor->getEdit()->getImageNow(),
                    QRect(xPos, yPos, w * str.size(), h));

        p.setFont(getSelectedFont(ui->fontAABox->isChecked()));

        for (int i = 0;i < str.size();i++) {
            if (ui->fontBorderBox->isChecked()) {
                p.setPen(Qt::darkRed);
                p.drawRect(i * w, 0, w - 1, h - 1);
            }
            p.setPen(Qt::white);
            p.drawText(QRect(i * w, 0, w, h), Qt::AlignCenter, str.at(i));
        }

        for (int i = 0;i < img.width();i++) {
            for (int j = 0;j < img.height();j++) {
                QColor c = img.pixelColor(i, j);
                if (c != Qt::darkRed) {
                    int gray = qGray(c.rgb());
                    gray /= 16;
                    img.setPixelColor(i, j, ui->fullEditor->paletteColor(gray));
                }
            }
        }

        ui->fullEditor->getEdit()->setOverlayImage(
                    xPos, yPos,
                    0, 128, 0, 128, 0, 0, 0, 0, 0, 1.0, Qt::black,
                    img);
    }

    mOverlayUpdating = false;
}

void MainWindow::on_actionShow_instruction_sheet_triggered()
{
    QMessageBox::information(this, "Usage Instructions",
                             "<b>Navigate in editor</b><br>"
                             "Left-click and drag to move. Scroll to zoom.<br>"
                             "<br>"
                             "<b>Draw pixels</b><br>"
                             "Shift + Left-click (and drag)<br>"
                             "<br>"
                             "<b>Change color</b><br>"
                             "Click on color buttons, or shift + right-click on pixel with desired color.<br>"
                             "<br>"
                             "<b>Overlay</b><br>"
                             "Overlay is an important function of this editor, which overlays an image "
                             "from the custom tab to the Full Frame tab with a transform. The interesting aspect "
                             "is that the same transforms are available on the controller, meaning that all transform "
                             "parameters can be animated. For example, if a rotating needle should show speed, "
                             "the needle can be drawn pointing to the right in the Custom tab and the rotation "
                             "can be tested here. The same rotation can be achieved on the remote. One can also "
                             "become creative with animating other properties of the overlay. To get information "
                             "about what the transform parameters do, the mouse can be hovered above the text to "
                             "the left of them.<br>"
                             "<br>"
                             "The custom tab supports images of any size, as long as the width and height is an "
                             "even number. The size is obviously limited by the memory on the microcontroller though. "
                             "Each image will consume <b>width * height / 2</b> bytes. There are around 800 KB total that "
                             "can be used for images.<br>"
                             "<br>"
                             "<b>Layer 2</b><br>"
                             "The second layer can be used to draw overlays on, without messing with the main image. "
                             "This way the background image can be kept clean for when it is saved.");
}

QFont MainWindow::getSelectedFont(bool antialias)
{
    QFont f = ui->fontBox->currentFont();
    f.setPointSizeF(500.0);
    f.setBold(ui->fontBoldBox->isChecked());

    if (!antialias) {
        f.setStyleStrategy(QFont::NoAntialias);
    }

    QFontMetrics fm(f);
    double fh = (500.0 * (double)ui->fontHBox->value()) / (double)fm.ascent();
    f.setPointSizeF(fh * ui->fontScaleBox->value());
    return f;
}

void MainWindow::on_exportFontButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save C Source File"), "",
                                                    tr("C source files (*.c *.C)"));

    if (filename.isEmpty()) {
        return;
    }

    if (!filename.toLower().endsWith(".c")) {
        filename.append(".c");
    }

    QFile f(filename);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    int w = ui->fontWBox->value();
    int h = ui->fontHBox->value();
    int bytesPerChar = (w * h) / 8;

    if ((w * h) % 8 != 0) {
        bytesPerChar++;
    }

    QByteArray font;
    font.resize(bytesPerChar * 95);

    for (auto &c: font) {
        c = 0;
    }

    for (int ch = 0;ch < 95;ch++) {
        QImage img(w, h, QImage::Format_ARGB32);
        QPainter p(&img);

        p.fillRect(img.rect(), Qt::black);
        p.setPen(Qt::white);

        p.setFont(getSelectedFont(false));
        p.drawText(QRect(0, 0, w, h), Qt::AlignCenter, QChar(ch + ' '));

        for (int i = 0;i < w * h;i++) {
            QColor px = img.pixel(i % w, i / w);
            char c = font[bytesPerChar * ch + i / 8];
            c |= (px != Qt::black) << (i % 8);
            font[bytesPerChar * ch + (i / 8)] = c;
        }
    }

    QTextStream out(&f);

    out << "/* \n";
    out << " * 1-bit Monochrome Font\n";
    out << " * Font size  : " << QString("%1x%2\n").arg(w).arg(h);
    out << " * Bytes total: " << QString("%1\n").arg(font.size());
    out << " */\n";

    out << QString("const uint8_t font_%1x%2[] = {\n").arg(w).arg(h);
    out << QString("%1, %2, // WxH\n").arg(w).arg(h);

    for (int ch = 0;ch < 95;ch++) {
        for (int i = 0;i < bytesPerChar;i++) {
            out << QString("0x%1, ").arg((uint8_t)(font.at(ch * bytesPerChar + i)),
                                         2, 16, QChar('0'));
        }

        QString chp = QChar(' ' + ch);

        if ((' ' + ch) == ' ') {
            chp = "<space>";
        } else if ((' ' + ch) == '\\') {
            chp = "<backslash>";
        }

        out << "// " << chp << "\n";
    }

    out << "};\n";
    out.flush();
    f.close();
}

void MainWindow::on_exportFontAAButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save C Source File"), "",
                                                    tr("C source files (*.c *.C)"));

    if (filename.isEmpty()) {
        return;
    }

    if (!filename.toLower().endsWith(".c")) {
        filename.append(".c");
    }

    QFile f(filename);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    int w = ui->fontWBox->value();
    int h = ui->fontHBox->value();
    int bytesPerChar = (w * h) / 2;

    if ((w * h) % 2 != 0) {
        bytesPerChar++;
    }

    QByteArray font;
    font.resize(bytesPerChar * 95);

    for (auto &c: font) {
        c = 0;
    }

    for (int ch = 0;ch < 95;ch++) {
        QImage img(w, h, QImage::Format_ARGB32);
        QPainter p(&img);

        p.fillRect(img.rect(), Qt::black);
        p.setPen(Qt::white);

        p.setFont(getSelectedFont(true));
        p.drawText(QRect(0, 0, w, h), Qt::AlignCenter, QChar(ch + ' '));

        for (int i = 0;i < w * h;i++) {
            QColor px = img.pixel(i % w, i / w);
            char c = font[bytesPerChar * ch + i / 2];

            if ((i % 2) == 0) {
                c |= (qGray(px.rgb()) / 16) & 0x0F;
            } else {
                c |= ((qGray(px.rgb()) / 16) << 4) & 0xF0;
            }

            font[bytesPerChar * ch + (i / 2)] = c;
        }
    }

    QTextStream out(&f);

    out << "/* \n";
    out << " * 4-bit Antialiased Font\n";
    out << " * Font size  : " << QString("%1x%2\n").arg(w).arg(h);
    out << " * Bytes total: " << QString("%1\n").arg(font.size());
    out << " */\n";

    out << QString("const uint8_t font_aa_%1x%2[] = {\n").arg(w).arg(h);
    out << QString("%1, %2, // WxH\n").arg(w).arg(h);

    for (int ch = 0;ch < 95;ch++) {
        for (int i = 0;i < bytesPerChar;i++) {
            out << QString("0x%1, ").arg((uint8_t)(font.at(ch * bytesPerChar + i)),
                                         2, 16, QChar('0'));
        }

        QString chp = QChar(' ' + ch);

        if ((' ' + ch) == ' ') {
            chp = "<space>";
        } else if ((' ' + ch) == '\\') {
            chp = "<backslash>";
        }

        out << "// " << chp << "\n";
    }

    out << "};\n";
    out.flush();
    f.close();
}
