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

#include "dispeditor.h"
#include "ui_dispeditor.h"

#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QColor>
#include <QImage>

DispEditor::DispEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DispEditor)
{
    ui->setupUi(this);

    updateSize(128, 128);

    ui->imageWidget->setPixmap(QPixmap::fromImage(ui->displayEdit->getImageNow()));

    connect(ui->displayEdit, &DisplayEdit::imageUpdated, [this](QImage image) {
        ui->imageWidget->setPixmap(QPixmap::fromImage(image));
    });

    QGridLayout *l = new QGridLayout;
    l->setVerticalSpacing(1);
    l->setHorizontalSpacing(1);
    l->setMargin(1);

    connect(ui->displayEdit, &DisplayEdit::editColorChanged, [this](QColor c) {
        QPixmap px(50, 50);
        QPainter painter(&px);
        painter.fillRect(0, 0, px.width(), px.height(), c);
        ui->ColorNowLabel->setPixmap(px);
    });

    for (int i = 0;i < 16;i++) {
        QColor c = QColor::fromRgbF((double)i / 15.0, (double)i / 15.0, (double)i / 15.0);
        QPushButton *b = new QPushButton();
        QPixmap px(50, 50);
        QPainter painter(&px);
        painter.fillRect(0, 0, px.width(), px.height(), c);
        b->setIcon(px);
        l->addWidget(b, 0, i);
        mPalette.append(c);

        if (i == 15) {
            ui->displayEdit->setEditColor(c);
        }

        connect(b, &QPushButton::clicked, [this,c,px](bool) {
            ui->displayEdit->setEditColor(c);
            ui->ColorNowLabel->setPixmap(px);
        });
    }

    ui->colorWidget->setLayout(l);
}

void DispEditor::updateSize(int width, int height)
{
    ui->displayEdit->setImageSize(width, height);
    ui->imageWidget->setMinimumWidth(width * 2);
    ui->imageWidget->setMaximumWidth(width * 2);
    ui->imageWidget->setMinimumHeight(height * 2);
    ui->imageWidget->setMaximumHeight(height * 2);
}

DisplayEdit *DispEditor::getEdit()
{
    return ui->displayEdit;
}

QColor DispEditor::paletteColor(int ind)
{
    if (ind >= 0 && ind < mPalette.size()) {
        return mPalette.at(ind);
    } else {
        return Qt::black;
    }
}

DispEditor::~DispEditor()
{
    delete ui;
}

void DispEditor::on_saveCButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save C Source File"), "",
                                                    tr("C source files (*.c *.C)"));

    if (!filename.isEmpty()) {
        if (!filename.toLower().endsWith(".c")) {
            filename.append(".c");
        }

        QFile f(filename);

        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return;
        }

        QTextStream out(&f);

        out << "const uint8_t image[] = {\n";

        QImage img = ui->displayEdit->getImageNow();

        int lineCnt = 0;
        for (int j = 0;j < img.height();j++) {
            for (int i = 0;i < ((img.width() + 1) / 2);i++) {
                uint8_t p1 = mPalette.indexOf(img.pixelColor(2 * i, j));
                uint8_t p2 = mPalette.indexOf(img.pixelColor(2 * i + 1, j));
                uint8_t p = (p1 & 0x0F) << 4 | (p2 & 0x0F);

                out << QString("0x%1, ").arg((qulonglong)p, 2, 16, QChar('0'));
                lineCnt++;
                if (lineCnt >= 24) {
                    out << "\n";
                    lineCnt = 0;
                }
            }
        }

        out << "\n};\n";
        out.flush();
        f.close();
    }
}

void DispEditor::on_savePngButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save PNG File"), "",
                                                    tr("PNG files (*.png *.Png *.PNG)"));

    if (!filename.isEmpty()) {
        if (!filename.toLower().endsWith(".png")) {
            filename.append(".png");
        }

        ui->displayEdit->getImageNow().save(filename, "PNG");
    }
}

void DispEditor::on_loadPngButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Load Image File"), "",
                                                    tr("JPG and PNG files (*.png *.Png *.PNG *.jpg *.Jpg *.JPG *.jpeg *.Jpeg *.JPEG)"));

    if (!filename.isEmpty()) {
        QImage img;
        img.load(filename);
        QImage img2(img.size(), QImage::Format_ARGB32);

        QPainter p(&img2);
        p.drawImage(0, 0, img);

        img = img2.scaled(ui->displayEdit->getImageSize(),
                         Qt::KeepAspectRatioByExpanding,
                         ui->antialiasBox->isChecked() ?
                             Qt::SmoothTransformation :
                             Qt::FastTransformation);

        bool validPalette = true;
        for (int i = 0;i < img.width();i++) {
            for (int j = 0;j < img.height();j++) {
                if (!mPalette.contains(img.pixelColor(i, j))) {
                    validPalette = false;
                    break;
                }
            }
        }

        if (!validPalette) {
            if (ui->ditherBox->isChecked()) {
                int imgW = img.width();
                int imgH = img.height();
                int bits = 4;

                int **img_buffer = new int*[imgW];
                for(int i = 0; i < imgW; i++) {
                    img_buffer[i] = new int[imgH];
                }

                int mask = (0xFF << (8 - bits)) & 0xFF;

                for (int y = 0;y < imgH;y++) {
                    for (int x = 0;x < imgW;x++) {
                        QRgb col = img.pixel(x, y);
                        img_buffer[x][y] = qGray(col);
                    }
                }

                for (int y = 0;y < imgH;y++) {
                    for (int x = 0;x < imgW;x++) {
                        int pix = img_buffer[x][y];

                        if (pix > 255) {
                            pix = 255;
                        }

                        if (pix < 0) {
                            pix = 0;
                        }

                        int pix_n = pix & mask;
                        int quant_error = pix - pix_n;
                        img_buffer[x][y]= pix_n;

                        if (x < (imgW - 1)) {
                            img_buffer[x + 1][y] += ((quant_error << 12) * 7) >> 16;
                        }


                        if (y < (imgH - 1)) {
                            img_buffer[x][y + 1] += ((quant_error << 12) * 5) >> 16;
                        }

                        if (x > 0 && y < (imgH - 1)) {
                            img_buffer[x - 1][y + 1] += ((quant_error << 12) * 3) >> 16;
                        }

                        if (x < (imgW - 1) && y < (imgH - 1)) {
                            img_buffer[x + 1][y + 1] += ((quant_error << 12) * 1) >> 16;
                        }
                    }
                }

                for (int y = 0;y < imgH;y++) {
                    for (int x = 0;x < imgW;x++) {
                        img.setPixelColor(x, y, mPalette.at(img_buffer[x][y] >> 4));
                    }
                }

                for(int i = 0; i < imgW; i++) {
                    delete[] img_buffer[i];
                }
                delete[] img_buffer;
            } else {
                for (int i = 0;i < img.width();i++) {
                    for (int j = 0;j < img.height();j++) {
                        QColor c = img.pixelColor(i, j);
                        int gray = qGray(c.rgb());
                        gray /= 16;
                        img.setPixelColor(i, j, mPalette.at(gray));
                    }
                }
            }
        }

        ui->displayEdit->loadFromImage(img);
    }
}

void DispEditor::on_showLayer2Box_toggled(bool checked)
{
    ui->displayEdit->setDrawLayer2(checked);
}

void DispEditor::on_clearLayer2Button_clicked()
{
    ui->displayEdit->clearLayer2();
}

void DispEditor::on_clearButton_clicked()
{
    ui->displayEdit->clearImage();
}
