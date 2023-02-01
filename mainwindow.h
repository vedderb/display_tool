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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_updateSizeButton_clicked();
    void on_ovSaveButton_clicked();
    void updateOverlay();
    void on_actionShow_instruction_sheet_triggered();
    void on_exportFontButton_clicked();
    void on_exportFontAAButton_clicked();
    void on_exportFontCustomButton_clicked();

private:
    Ui::MainWindow *ui;
    QFont getSelectedFont(bool antialias);

    bool mOverlayUpdating;

};

#endif // MAINWINDOW_H
