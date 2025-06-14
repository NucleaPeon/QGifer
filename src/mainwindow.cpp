/************************************************************************
** QGifer
** Copyright (C) 2013 Lukasz Chodyla <chodak166@op.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
************************************************************************/

#include <QTimer>
#include <QProgressDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QTextStream>
#include <QDirIterator>
#include <QDesktopWidget>

#include "widgets/QPressAction.h"
#include "mainwindow.h"
#include "objectwidget.h"

MainWindow::MainWindow() : translator(NULL), locked(false) {
    setupUi(this);
    //player->controlPanel->hide();
    player->setStatusBar(statusbar);
    renderDefaultText();
    //player->showDefaultScreen();
    set = new QSettings("QGifer", "QGifer");
    set->setIniCodec("UTF-8");
    // upStartButton->setIcon(QIcon(":/res/fromimg.png"));
    // upStartButton->setDefaultAction(actionSetAsStart);
    // upStopButton->setDefaultAction(actionSetAsStop);
    // upStopButton->setIcon(QIcon(":/res/fromimg.png"));

    actionNextFrame = new QPressAction(this);
    actionNextFrame->setObjectName(QString::fromUtf8("actionNextFrame"));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/res/next.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionNextFrame->setIcon(icon4);
    actionPrevFrame = new QPressAction(this);
    actionPrevFrame->setObjectName(QString::fromUtf8("actionPrevFrame"));
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/res/prev.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionPrevFrame->setIcon(icon5);

    playerToolBar->addAction(actionPrevFrame);
    playerToolBar->addAction(actionNextFrame);

    QWidget *widgetForAction = playerToolBar->widgetForAction(actionNextFrame);
    widgetForAction->setMouseTracking(true);
    widgetForAction->installEventFilter(actionNextFrame);

    widgetForAction = playerToolBar->widgetForAction(actionPrevFrame);
    widgetForAction->setMouseTracking(true);
    widgetForAction->installEventFilter(actionPrevFrame);

    // TODO Move to default constructor

    multiSlider->setSkin(QPixmap(":/res/multislider/bar.png"),
                         QPixmap(":/res/multislider/midbar.png"),
                         QPixmap(":/res/multislider/left.png"),
                         QPixmap(":/res/multislider/right.png"),
                         QPixmap(":/res/multislider/a.png").
                                 scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation),
                         QPixmap(":/res/multislider/b.png").
                                 scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    multiSlider->setMaximum(0);
    multiSlider->setPosA(0);
    multiSlider->setPosB(0);

    if (checkFFMPEG()) {
        connect(actionConverter, SIGNAL(triggered()), this, SLOT(runConverter()));
    }
    else {
        actionConverter->setVisible(false);
    }

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionOptimizer, SIGNAL(triggered()), this, SLOT(runOptimizer()));
    connect(actionExtractGif, SIGNAL(triggered()), this, SLOT(extractGif()));
    //connect(updatePalButton, SIGNAL(clicked()), this, SLOT(updatePalette()));
    connect(actionUpdatePalette, SIGNAL(triggered()), this, SLOT(updatePalette()));

    connect(actionSetAsStart, SIGNAL(triggered()), this, SLOT(startFromCurrent()));
    connect(actionSetAsStop, SIGNAL(triggered()), this, SLOT(stopFromCurrent()));

    connect(actionJumpToStart, SIGNAL(triggered()), this, SLOT(jumpToStart()));
    connect(actionJumpToStop, SIGNAL(triggered()), this, SLOT(jumpToStop()));

    connect(actionSavePalette, SIGNAL(triggered()), this, SLOT(savePalette()));
    connect(actionOpenPalette, SIGNAL(triggered()), this, SLOT(openPalette()));
    connect(actionInsertObject, SIGNAL(triggered()), this, SLOT(insertObject()));
    connect(actionInsertText, SIGNAL(triggered()), this, SLOT(insertText()));

    connect(actionNewProject, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(actionOpenProject, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(actionSaveProject, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(actionSaveProjectAs, SIGNAL(triggered()), this, SLOT(saveProjectAs()));

    connect(actionOpenVideo, SIGNAL(triggered()), this, SLOT(openVideo()));
    connect(actionCloseVideo, SIGNAL(triggered()), player, SLOT(close()));
    connect(actionPlay, SIGNAL(triggered()), player, SLOT(play()));
    connect(actionStop, SIGNAL(triggered()), player, SLOT(stop()));
    connect(actionPause, SIGNAL(triggered()), player, SLOT(pause()));

    connect(actionNextFrame, SIGNAL(press_start()), player, SLOT(play()));
    connect(actionNextFrame, SIGNAL(press_end()), player, SLOT(pause()));
    connect(actionNextFrame, SIGNAL(triggered()), player, SLOT(nextFrame()));

    connect(actionPrevFrame, SIGNAL(triggered()), player, SLOT(prevFrame()));
    connect(actionPrevFrame, SIGNAL(press_start()), player, SLOT(reverse_play()));
    connect(actionPrevFrame, SIGNAL(press_end()), player, SLOT(pause()));

    connect(actionRestoreDefault, SIGNAL(triggered()), this, SLOT(restoreDefault()));

    connect(stopBox, SIGNAL(editingFinished()), this, SLOT(stopChanged()));
    connect(startBox, SIGNAL(editingFinished()), this, SLOT(startChanged()));
    connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(outputWidthChanged(int)));
    connect(heightBox, SIGNAL(valueChanged(int)), this, SLOT(outputHeightChanged(int)));

    connect(multiSlider, SIGNAL(posAChanged(int)), this, SLOT(posAChanged(int)));
    connect(multiSlider, SIGNAL(posBChanged(int)), this, SLOT(posBChanged(int)));

    connect(fpsBox, SIGNAL(valueChanged(double)), player, SLOT(setFps(double)));
    connect(player, SIGNAL(frameChanged(long)), this, SLOT(frameChanged(long)));

    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomChanged(int)));
    connect(ratioBox, SIGNAL(stateChanged(int)), this, SLOT(ratioChanged(int)));
    connect(smoothBox, SIGNAL(stateChanged(int)), this, SLOT(smoothChanged(int)));
    connect(marginBox, SIGNAL(stateChanged(int)), this, SLOT(marginBoxChanged(int)));
    connect(varPaletteBox, SIGNAL(stateChanged(int)), this, SLOT(varPaletteBoxChanged(int)));
    connect(autoPaletteBox, SIGNAL(stateChanged(int)), this, SLOT(autoPaletteBoxChanged(int)));
    connect(ditherBox, SIGNAL(stateChanged(int)), this, SLOT(setChanged()));

    connect(hueSlider, SIGNAL(valueChanged(int)), this, SLOT(correctionChanged()));
    connect(satSlider, SIGNAL(valueChanged(int)), this, SLOT(correctionChanged()));
    connect(valSlider, SIGNAL(valueChanged(int)), this, SLOT(correctionChanged()));
    connect(correctionBox, SIGNAL(stateChanged(int)), this, SLOT(correctionChanged()));
    connect(filterObjBox, SIGNAL(stateChanged(int)), this, SLOT(correctionChanged()));
    connect(resetCorrectionButton, SIGNAL(clicked()), this, SLOT(resetCorrection()));
    connect(medianSlider, SIGNAL(valueChanged(int)), this, SLOT(medianChanged(int)));

    connect(getWHButton, SIGNAL(clicked()), this, SLOT(estimateOutputSize()));
    connect(whRatioBox, SIGNAL(toggled(bool)), this, SLOT(whRatioChanged(bool)));
    connect(player->getWorkspace(), SIGNAL(objectChanged()), this, SLOT(setChanged()));
    connectMargins();

    connect(player->getWorkspace(), SIGNAL(marginsChanged()), this, SLOT(marginsChanged()));
    connect(player->getWorkspace(), SIGNAL(propertiesRequested(WorkspaceObject * )),
            this, SLOT(showProperties(WorkspaceObject * )));

    connect(toolDock, SIGNAL(topLevelChanged(bool)), this, SLOT(dockLevelChanged(bool)));

    connect(actionOutputProp, SIGNAL(triggered()), this, SLOT(showOutputProp()));
    connect(actionFilters, SIGNAL(triggered()), this, SLOT(showFilters()));
    connect(actionMargins, SIGNAL(triggered()), this, SLOT(showMargins()));
    connect(actionPreviewProp, SIGNAL(triggered()), this, SLOT(showPreviewProp()));
    connect(actionUndock, SIGNAL(triggered()), this, SLOT(toggleDock()));
    connect(actionDrawBkg, SIGNAL(toggled(bool)),
            this, SLOT(drawBkgToggled(bool)));

    connect(menuLanguage, SIGNAL(triggered(QAction * )), this, SLOT(languageChanged(QAction * )));
    connect(player->getWorkspace(), SIGNAL(wheelRotated(int)),
            this, SLOT(workspaceWheelRotated(int)));

    toolBox->setMinimumWidth(270);
    loadSettings();
    changed = false;
    newProject();

}

MainWindow::~MainWindow() {
    delete set;
    delete translator;
}

void MainWindow::openVideo() {
    QString path = QFileDialog::getOpenFileName(
            this, tr("Open video file"),
            set->value("last_video_dir", "").toString(),
            "Video files (*.avi *.mp4 *.mpg *.ogv);;All files (*.*)");
    if (!path.isEmpty()) if (!openVideo(path)) {
        QMessageBox::critical(this, tr("Error"), tr("The player failed to load the video file!"));
    }
    QList<WorkspaceObject *> *ol = player->getWorkspace()->getObjectsList();
    for (int i = 0; i < ol->size(); i++)
        if (ol->at(i)->getStart() >= player->countFrames()) {
            QString name = ol->at(i)->getTypeName() == "TextObject" ?
                           static_cast<TextObject *>(ol->at(i))->getText() : ol->at(i)->getImagePath();
            int q = QMessageBox::question(
                    this, tr("Question"),
                    tr("The object \"") + name + tr("\" will not be visible. What do you want to do?"),
                    tr("Delete this object"), tr("Change frame range..."));
            if (!q) {
                ol->removeAt(i);
                i--;
            }
            else
                showProperties(ol->at(i));
        }
    player->getWorkspace()->update();
}

bool MainWindow::openVideo(const QString &path) {
    if (player->openSource(path)) {
        startBox->setMaximum(player->countFrames() - 1);
        stopBox->setMaximum(player->countFrames() - 1);
        multiSlider->setMaximum(player->countFrames() - 1);
        startBox->setValue(0);
        stopBox->setValue(1);
        fpsBox->setValue(player->fps());
        set->setValue("last_video_dir", QFileInfo(path).absoluteDir().absolutePath());
        vidfile = path;
        lock(false);
        setChanged();
        whRatioChanged(ratioBox->isChecked());
        return true;
    }
    startBox->setMaximum(0);
    stopBox->setMaximum(0);

    player->showDefaultScreen();
    lock(true);

    return false;
}


void MainWindow::extractGif() {
    qDebug() << Q_FUNC_INFO;
    if (startBox->value() > stopBox->value()) {
        QMessageBox::critical(this, tr("Error"), tr("The range is invalid!"));
        return;
    }

    if (!paletteWidget->map() || autoPaletteBox->isChecked()) {
        player->seek(startBox->value());
        updatePalette();
    }

    if (!paletteWidget->map()) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid color map!"));
        return;
    }

    if (paletteWidget->map()->ColorCount > pow(2, paletteBox->value()))
        updatePalette();

    player->pause();

    GifWidget *gw = new GifWidget(set);
    gw->setAttribute(Qt::WA_DeleteOnClose, true);
    gw->setColorRes(paletteBox->value());

    gw->setVisibleFPS(player->fps());


    connect(gw, SIGNAL(gifSaved(QString)), this, SLOT(gifSaved(QString)));
    QString sn = QFileInfo(vidfile).baseName() + "_" +
                 QString::number(startBox->value()) + "-" +
                 QString::number(stopBox->value());
    gw->suggestName(sn);
    gw->setWindowTitle(sn);
    QProgressDialog pd(
                "Rendering frames...",
                "Abort",
                 startBox->value(),
                 stopBox->value(),
                       this);

    pd.setAttribute(Qt::WA_DeleteOnClose, true);
    pd.setWindowModality(Qt::WindowModal);    
    pd.show();
    qApp->processEvents();
    ColorMapObject *map = paletteWidget->mapCopy();
    player->seek(startBox->value() - 1);
    for (long i = startBox->value(); i <= stopBox->value() && ! pd.wasCanceled(); i++) {
        pd.setValue(i);
        if (varPaletteBox->isChecked()) {
            player->nextFrame();
            float diff = minDiffBox->value() / 100.0f;
            if (i == startBox->value()) {
                updatePalette();
                diff = 0;
            }

            paletteWidget->fromImage(
                    finalFrame(player->getCurrentPos()),
                    pow(2, paletteBox->value()),
                    diff
            );
            map = paletteWidget->mapCopy();
            gw->addFrame(finalFrame(i), map, ditherBox->isChecked());
        }
        else {
            player->nextFrame();
            gw->addFrame(finalFrame(i),
                         i == startBox->value() ? map : NULL,
                         ditherBox->isChecked());
        }

    }
    pd.setValue(stopBox->value());

    gw->move(
             x() + width() / 2 - gw->minimumSize().width() / 2,
             y() + height() / 2 - gw->minimumSize().height() / 2);
    gw->show();
    gw->play();
    player->pause();
    QTimer::singleShot(100, gw, SLOT(adjustWidgetSize())); //TODO find another way....
}

void MainWindow::updatePalette() {
    paletteWidget->fromImage(finalFrame(player->getCurrentPos()), pow(2, paletteBox->value()));
    setChanged();
}

void MainWindow::posAChanged(int v) {
    startBox->setValue(v);
    if (autoPaletteBox->isChecked()) {
        player->seek(v);
        updatePalette();
    }
    setChanged();
}

void MainWindow::posBChanged(int v) {
    stopBox->setValue(v);
    setChanged();
}

void MainWindow::correctRange() {
    if (startBox->value() > stopBox->value()) {
        stopBox->setValue(startBox->value());
        stopChanged();
    }
}

void MainWindow::frameChanged(long f) {

    if (player->getStatus() == FramePlayer::Playing &&
        laRadio->isChecked() && f >= player->countFrames() - 1)
        player->seek(0);
    else if (ssRadio->isChecked() && player->getStatus() == FramePlayer::Playing &&
             (f >= stopBox->value() || f < startBox->value()) &&
             stopBox->value() >= startBox->value()) {
        player->seek(startBox->value());
        if (stopBox->value() == startBox->value()) {
            player->pause();
        }
    }
    else if (ssRadio->isChecked() && f >= stopBox->value() &&
             stopBox->value() < startBox->value()) {
        QMessageBox::warning(this, tr("Warning"), tr("The range seems to be invalid!"));
        player->pause();
    }


    if (correctionBox->isChecked() &&
        (hueSlider->value() || valSlider->value() || satSlider->value())) {
        bool c = isChanged();
        correctionChanged();
        setChanged(c);
    }
}

void MainWindow::gifSaved(const QString &path) {
    set->setValue("last_gif", path);
    set->setValue("last_gif_dir", QFileInfo(path).absoluteDir().absolutePath());
    if (set->value("show_optimizer", false).toBool()){
        runOptimizer();
    }
}

void MainWindow::zoomChanged(int v) {
    zoomLabel->setText(tr("Zoom") + " (" + QString::number(v) + "%):");
    player->getWorkspace()->setZoom((double) v / 100.0);
    if (player->getStatus() != FramePlayer::Playing) {
        player->seek(player->getCurrentPos());
    }
}

void MainWindow::ratioChanged(int s) {
    player->getWorkspace()->keepAspectRatio(s == Qt::Checked);
    if (player->getStatus() != FramePlayer::Playing) {
        player->seek(player->getCurrentPos());
    }
    setChanged();
}

void MainWindow::smoothChanged(int s) {
    player->getWorkspace()->enableAntialiasing(s == Qt::Checked);
    if (player->getStatus() != FramePlayer::Playing)
        player->seek(player->getCurrentPos());
}

void MainWindow::applyMargins() {
    *player->getWorkspace()->margins() = QMargins(
            leftSpin->value(), topSpin->value(),
            rightSpin->value(), bottomSpin->value());
    setChanged();
    player->getWorkspace()->update();
}

void MainWindow::marginsChanged() {
    disconnectMargins();
    QMargins *m = player->getWorkspace()->margins();
    leftSpin->setValue(m->left());
    rightSpin->setValue(m->right());
    topSpin->setValue(m->top());
    bottomSpin->setValue(m->bottom());

    if (correctionBox->isChecked())
        correctionChanged();

    setChanged();
    connectMargins();
}

void MainWindow::connectMargins() {
    connect(leftSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
    connect(topSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
    connect(rightSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
    connect(bottomSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
}

void MainWindow::disconnectMargins() {
    disconnect(leftSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
    disconnect(topSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
    disconnect(rightSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
    disconnect(bottomSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
}

void MainWindow::marginBoxChanged(int s) {
    player->getWorkspace()->enableMargins(s == Qt::Checked);
    applyMargins();
    player->getWorkspace()->update();
    setChanged();
}

void MainWindow::correctionChanged() {
    hueLabel->setText(tr("Hue (") + QString::number(hueSlider->value()) + "):");
    satLabel->setText(tr("Saturation (") + QString::number(satSlider->value()) + "):");
    valLabel->setText(tr("Brightness (") + QString::number(valSlider->value()) + "):");

    if (!correctionBox->isChecked()) {
        player->getWorkspace()->setImage(*player->getCurrentFrame(), player->frame->size());
        player->getWorkspace()->enableAutoObjectDrawing(true);
        player->getWorkspace()->update();
        return;
    }

    QImage fp = *player->getCurrentFrame(); //tymczasowa kopia, bez niej mallock crash
    if (filterObjBox->isChecked()) {
        player->getWorkspace()->enableFiltering(
                hueSlider->value(),
                satSlider->value(),
                valSlider->value());
        player->getWorkspace()->drawObjects(&fp);
        player->getWorkspace()->enableAutoObjectDrawing(false);
    }
    else {
        player->getWorkspace()->disableFiltering();
        player->getWorkspace()->enableAutoObjectDrawing(true);
    }

    QRect marginRect = QRect(leftSpin->value() - 1,
                             topSpin->value() - 1,
                             fp.width() - leftSpin->value() - rightSpin->value() + 2,
                             fp.height() - topSpin->value() - bottomSpin->value() + 2);
    PreviewWidget::applyCorrection(&fp,
                                   hueSlider->value(),
                                   satSlider->value(),
                                   valSlider->value(),
                                   true, marginRect);

    player->getWorkspace()->setImage(fp, player->frame->size());
    player->getWorkspace()->update();
    setChanged();
}

void MainWindow::resetCorrection() {
    hueSlider->setValue(0);
    satSlider->setValue(0);
    valSlider->setValue(0);
}

void MainWindow::lock(bool l) {
    locked = l;
    l = !l;
    toolBox->setEnabled(l);
    multiSlider->setEnabled(l);
    actionSaveProject->setEnabled(l);
    actionSaveProjectAs->setEnabled(l);
    actionPlay->setEnabled(l);
    actionStop->setEnabled(l);
    actionPause->setEnabled(l);
    actionNextFrame->setEnabled(l);
    actionPrevFrame->setEnabled(l);
    actionSetAsStart->setEnabled(l);
    actionSetAsStop->setEnabled(l);
    actionExtractGif->setEnabled(l);
    actionInsertText->setEnabled(l);
    actionInsertObject->setEnabled(l);

    actionOutputProp->setEnabled(l);
    actionPreviewProp->setEnabled(l);
    actionMargins->setEnabled(l);
    actionFilters->setEnabled(l);
    actionDrawBkg->setEnabled(l);

    actionOpenPalette->setEnabled(l);
    actionSavePalette->setEnabled(l);
    actionUpdatePalette->setEnabled(l);
    player->getWorkspace()->setMarginsVisible(l);

    player->getWorkspace()->enableBackground(
            l ? actionDrawBkg->isChecked() : true);

    varPaletteBoxChanged(varPaletteBox->checkState());
    autoPaletteBoxChanged(autoPaletteBox->checkState());
}

QImage MainWindow::finalFrame(long f) {
    heightBox->disconnect();
    widthBox->disconnect();
    if (heightBox->value() % 2) {
        heightBox->setValue(heightBox->value() + 1); //nieparzyste powodowaly bugi (?)
    }


    ow = widthBox->value();
    oh = heightBox->value();
    //qDebug() << "final frame, ow x oh: " << ow << " x " << oh;

    if (f != player->getCurrentPos()) {
        player->seek(f);
    }

    QImage frame = *player->getCurrentFrame();
    QSize s = player->getCurrentFrame()->size();

    //rysowanie obiektow przed filtrowaniem
    if (filterObjBox->isChecked())
        player->getWorkspace()->drawObjects(&frame);

    QRect marginRect = QRect(leftSpin->value(),
                             topSpin->value(),
                             s.width() - leftSpin->value() - rightSpin->value(),
                             s.height() - topSpin->value() - bottomSpin->value());

    if (hueSlider->value() || valSlider->value() || satSlider->value()) {
        PreviewWidget::applyCorrection(
                &frame,
                hueSlider->value(),
                satSlider->value(),
                valSlider->value(),
                true, marginRect);
        //qDebug() << "corrected image format: " << frame.format();
    }

    //rysowanie obiektow po filtrowaniu
    if (!filterObjBox->isChecked()) {
        player->getWorkspace()->drawObjects(&frame);
    }
    //marginesy
    if (marginBox->isChecked()) {
        frame = frame.copy(marginRect);
    }

    //skalowanie
    frame = frame.scaled(ow, oh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //qDebug() << "skalowane do " << ow <<"x"<<oh<<", wyszlo: " << frame.size();
    //frame = frame.scaled(ow,oh);


/* ##### PRZY SMOOTH TRANSFORMATION BLAD ZDAJE SIE NIE WYSTEPOWAC #######

     //--- korekcja rozdzielczosci, przy niektorych wartosciach wystepuje bug
     //--- i gif jest przekrzywiony. Liczba byteCount() jest większa od w*h*3
     //--- o wielokrotnosc h... wyrownamy wiec roznice na podstawie pierwszej klatki
     if(oh%2) oh++;
     // qDebug() << "\ncorrecting resolution....";
     qDebug() << "byte count: " << frame.byteCount();
     qDebug() << "w*h*3: " << (frame.width()*frame.height()*3);
     int diff = frame.byteCount()-(frame.width()*frame.height()*3);
     qDebug() << "byte difference: " << diff;
     ow = ow+diff/oh;
     // qDebug() << "corrected.\n";

     if(diff)
     {
      qDebug() << "new size recursion...";
      widthBox->setValue(ow);
      heightBox->setValue(oh);
      qApp->processEvents();
      return finalFrame(f);
     }

*/


    connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(outputWidthChanged(int)));
    connect(heightBox, SIGNAL(valueChanged(int)), this, SLOT(outputHeightChanged(int)));

    return frame.format() == QImage::Format_RGB888 ? frame :
           frame.convertToFormat(QImage::Format_RGB888);
}

void MainWindow::loadSettings() {
    //zoomSlider->setValue(set->value("zoom",100).toInt());
    smoothBox->setChecked(set->value("smooth_preview", true).toBool());
    ratioBox->setChecked(set->value("keep_ratio", true).toBool());
    switch (set->value("loop", 0).toInt()) {
        case 1:
            laRadio->setChecked(true);
            break;
        case 2:
            ssRadio->setChecked(true);
            break;
        default:
            nlRadio->setChecked(true);
    }
    marginBox->setChecked(set->value("use_margins", true).toBool());
    // leftSpin->setValue(set->value("left_margin",10).toInt());
    // rightSpin->setValue(set->value("right_margin",10).toInt());
    // topSpin->setValue(set->value("top_margin",10).toInt());
    // bottomSpin->setValue(set->value("bottom_margin",10).toInt());
    widthBox->setValue(set->value("output_width", 320).toInt());
    heightBox->setValue(set->value("output_height", 240).toInt());
    paletteBox->setValue(set->value("palette_size", 10).toInt());
    autoPaletteBox->setChecked(set->value("auto_palette", true).toBool());
    whRatioBox->setChecked(set->value("wh_ratio", false).toBool());
    varPaletteBox->setChecked(set->value("var_palette", false).toBool());
    minDiffBox->setValue(set->value("vp_mindiff", 40).toFloat());
    actionDrawBkg->setChecked(set->value("draw_bkg", true).toBool());
    ditherBox->setChecked(set->value("dither", true).toBool());
    resetCorrection();
    restoreState(set->value("windowstate").toByteArray());
    restoreGeometry(set->value("geometry").toByteArray());
    loadLanguages();
    QString qm = set->value("qmfile", "").toString();
    if (!qm.isEmpty() && QFile::exists(dataDir() + "/locale/" + qm + ".qm"))
        loadLanguage(qm, dataDir() + "/locale/");
}

void MainWindow::saveSettings() {
    //set->setValue("zoom",zoomSlider->value());
    set->setValue("smooth_preview", smoothBox->isChecked());
    set->setValue("keep_ratio", ratioBox->isChecked());
    set->setValue("loop", (laRadio->isChecked() ? 1 : ssRadio->isChecked() ? 2 : 0));
    set->setValue("use_margins", marginBox->isChecked());
    // set->setValue("left_margin",leftSpin->value());
    // set->setValue("right_margin",rightSpin->value());
    // set->setValue("top_margin",topSpin->value());
    // set->setValue("bottom_margin",bottomSpin->value());
    set->setValue("output_width", widthBox->value());
    set->setValue("output_height", heightBox->value());
    set->setValue("palette_size", paletteBox->value());
    set->setValue("auto_palette", autoPaletteBox->isChecked());
    set->setValue("wh_ratio", whRatioBox->isChecked());
    set->setValue("var_palette", varPaletteBox->isChecked());
    set->setValue("vp_mindiff", minDiffBox->value());
    set->setValue("windowstate", saveState());
    set->setValue("geometry", saveGeometry());
    set->setValue("draw_bkg", actionDrawBkg->isChecked());
    set->setValue("dither", ditherBox->isChecked());
}

void MainWindow::estimateOutputSize() {

    disconnect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(outputWidthChanged(int)));
    disconnect(heightBox, SIGNAL(valueChanged(int)), this, SLOT(outputHeightChanged(int)));

    QSize s = player->getCurrentFrame()->size();
    if (marginBox->isChecked()) {
        s.setWidth(s.width() - leftSpin->value() - rightSpin->value());
        s.setHeight(s.height() - topSpin->value() - bottomSpin->value());
    }

    float tmp = whRatio;
    whRatio = -1;
    widthBox->setValue(s.width());
    heightBox->setValue(s.height());
    whRatio = tmp;
    setChanged();
    whRatioChanged(ratioBox->isChecked());

    connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(outputWidthChanged(int)));
    connect(heightBox, SIGNAL(valueChanged(int)), this, SLOT(outputHeightChanged(int)));
}

QString MainWindow::projectToXml() {
    // TODO move to settings wrapper
    QString xcontent;
    QXmlStreamWriter stream(&xcontent);
    stream.setCodec("UTF-8");
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("qgifer_project");
    //zastepujemy sciezke do projektu przez %project_path% jezeli
    //sciezka do pliku wideo jest w (pod)katalogu projektu
    stream.writeAttribute("sourcevideo", projectRelativePath(player->source()));
    stream.writeAttribute("startframe", QString::number(startBox->value()));
    stream.writeAttribute("stopframe", QString::number(stopBox->value()));

    stream.writeAttribute("out_width", QString::number(widthBox->value()));
    stream.writeAttribute("out_height", QString::number(heightBox->value()));
    stream.writeAttribute("out_ratio", whRatioBox->isChecked() ? "true" : "false");
    stream.writeAttribute("var_palette", varPaletteBox->isChecked() ? "true" : "false");
    stream.writeAttribute("auto_palette", autoPaletteBox->isChecked() ? "true" : "false");
    stream.writeAttribute("dither", ditherBox->isChecked() ? "true" : "false");
    stream.writeAttribute("palette_size", QString::number(paletteBox->value()));
    stream.writeAttribute("mindiff", QString::number(minDiffBox->value()));

    stream.writeStartElement("margins");
    stream.writeAttribute("use_margins", marginBox->isChecked() ? "true" : "false");
    stream.writeAttribute("top", QString::number(topSpin->value()));
    stream.writeAttribute("right", QString::number(rightSpin->value()));
    stream.writeAttribute("bottom", QString::number(bottomSpin->value()));
    stream.writeAttribute("left", QString::number(leftSpin->value()));
    stream.writeEndElement(); //margins

    stream.writeStartElement("filters");
    stream.writeAttribute("preview", correctionBox->isChecked() ? "true" : "false");
    stream.writeAttribute("filter_objects", filterObjBox->isChecked() ? "true" : "false");
    stream.writeAttribute("hue", QString::number(hueSlider->value()));
    stream.writeAttribute("sat", QString::number(satSlider->value()));
    stream.writeAttribute("val", QString::number(valSlider->value()));
    stream.writeAttribute("median", QString::number(medianSlider->value()));
    stream.writeEndElement(); //filters


    if (!varPaletteBox->isChecked() && paletteWidget->map()) {
        stream.writeStartElement("palette");
        stream.writeCharacters(paletteWidget->toString());
        stream.writeEndElement();
    }


    QList<WorkspaceObject *> *objects = player->getWorkspace()->getObjectsList();

    for (int i = objects->size() - 1; i >= 0; i--) {
        const WorkspaceObject *o = objects->at(i);
        bool isText = o->getTypeName() == "TextObject";
        stream.writeStartElement("object");
        stream.writeAttribute("type", isText ? "text" : "image");
        stream.writeAttribute("start", QString::number(o->getStart()));
        stream.writeAttribute("stop", QString::number(o->getStop()));

        if (isText) {
            const TextObject *to = static_cast<const TextObject *>(o);
            stream.writeAttribute("text", to->getText());
            stream.writeAttribute("font", to->getFont().toString());
            stream.writeAttribute("outline_width", QString::number(to->getOutlineWidth()));
            stream.writeAttribute("textcolor", to->getTextColor().name());
            stream.writeAttribute("outlinecolor", to->getOutlineColor().name());
        }
        else
            stream.writeAttribute("image", projectRelativePath(o->getImagePath()));

        for (int p = o->getStart(); p <= o->getStop(); p++) {
            stream.writeStartElement("pos");
            const WOPos &pos = o->posAt(p);
            stream.writeAttribute("x", QString::number(pos.x));
            stream.writeAttribute("y", QString::number(pos.y));
            stream.writeEndElement();
        }
        for (int s = o->getStart(); s <= o->getStop(); s++) {
            stream.writeStartElement("scale");
            const WOSize &scale = o->scaleAt(s);
            stream.writeAttribute("w", QString::number(scale.w));
            stream.writeAttribute("h", QString::number(scale.h));
            stream.writeEndElement();
        }

        stream.writeEndElement();
    }

    stream.writeEndElement(); //qgifer_project
    stream.writeEndDocument();
    //qDebug() << "xml: " << xcontent;
    return xcontent;
}

bool MainWindow::projectFromXml(const QString &xstr) {
    player->getWorkspace()->getObjectsList()->clear();
    player->close();
    paletteWidget->clear();
    //clearCache();
    QXmlStreamReader stream(xstr);

    WorkspaceObject *currentObject = NULL;
    int posstart = 0, scalestart = 0;
    QString lastName;
    while (!stream.atEnd()) {
        stream.readNext();
        if (stream.isStartElement()) {
            //qDebug() << "START ELEMENT NAME: " << stream.name();
            if (stream.name() == "qgifer_project") {
                QString srcpath = stream.attributes().value("sourcevideo").toString().
                        replace("%project_path%", projectDir());
                if (!QFile::exists(srcpath) && !srcpath.isEmpty())
                    QMessageBox::warning(
                            this, tr("Warning"), tr("Source video file: ") + srcpath + " not found");
                else
                    openVideo(srcpath);

                qDebug() << "loaded width: " << stream.attributes().value("out_width").toString().toInt();

                startBox->setValue(0);
                stopBox->setValue(
                        stream.attributes().value("stopframe").toString().toInt());
                startBox->setValue(
                        stream.attributes().value("startframe").toString().toInt());
                startChanged();
                stopChanged();
                widthBox->setValue(
                        stream.attributes().value("out_width").toString().toInt());
                heightBox->setValue(
                        stream.attributes().value("out_height").toString().toInt());
                paletteBox->setValue(
                        stream.attributes().value("palette_size").toString().toInt());
                minDiffBox->setValue(
                        stream.attributes().value("mindiff").toString().toInt());
                whRatioBox->setChecked(
                        stream.attributes().value("out_ratio").toString() == "true");
                autoPaletteBox->setChecked(
                        stream.attributes().value("auto_palette").toString() == "true");
                varPaletteBox->setChecked(
                        stream.attributes().value("var_palette").toString() == "true");
                ditherBox->setChecked(
                        stream.attributes().value("dither").toString() == "true");
            }
            else if (stream.name() == "palette") {
                paletteWidget->fromString(stream.readElementText());
            }
            else if (stream.name() == "margins") {
                marginBox->setChecked(
                        stream.attributes().value("use_margins").toString() == "true");
                topSpin->setValue(
                        stream.attributes().value("top").toString().toInt());
                leftSpin->setValue(
                        stream.attributes().value("left").toString().toInt());
                bottomSpin->setValue(
                        stream.attributes().value("bottom").toString().toInt());
                rightSpin->setValue(
                        stream.attributes().value("right").toString().toInt());
            }
            else if (stream.name() == "filters") {
                correctionBox->setChecked(
                        stream.attributes().value("preview").toString() == "true");
                filterObjBox->setChecked(
                        stream.attributes().value("filter_objects").toString() == "true");
                hueSlider->setValue(
                        stream.attributes().value("hue").toString().toInt());
                satSlider->setValue(
                        stream.attributes().value("sat").toString().toInt());
                valSlider->setValue(
                        stream.attributes().value("val").toString().toInt());
                medianSlider->setValue(
                        stream.attributes().value("median").toString().toInt());
            }
            else if (stream.name() == "object") {
                if (stream.attributes().value("type").toString() == "text") {
                    qDebug() << "new text object...";
                    currentObject = new TextObject();
                    TextObject *to = static_cast<TextObject *>(currentObject);
                    to->setText(stream.attributes().value("text").toString());
                    QFont f;
                    f.fromString(stream.attributes().value("font").toString());
                    to->setFont(f);
                    to->setTextColor(
                            QColor(stream.attributes().value("textcolor").toString()));
                    to->setOutlineColor(
                            QColor(stream.attributes().value("outlinecolor").toString()));
                    to->setOutlineWidth(
                            stream.attributes().value("outline_width").toString().toInt());
                    TextRenderer::renderText(to);
                }
                else {
                    qDebug() << "new image object...";
                    currentObject = new WorkspaceObject();
                    currentObject->setImage(stream.attributes().value("image").toString().
                            replace("%project_path%", projectDir()));
                }

                posstart = scalestart = stream.attributes().value("start").toString().toInt();
                currentObject->setRange(posstart,
                                        stream.attributes().value("stop").toString().toInt());
                player->getWorkspace()->addObject(currentObject);

            }
            else if (stream.name() == "pos") {
                //qDebug() << "addig position at " << posstart;
                currentObject->setPosAt(posstart++,
                                        stream.attributes().value("x").toString().toDouble(),
                                        stream.attributes().value("y").toString().toDouble());
            }
            else if (stream.name() == "scale") {
                //qDebug() << "addig scale at " << scalestart;
                currentObject->setScaleAt(scalestart++,
                                          stream.attributes().value("w").toString().toDouble(),
                                          stream.attributes().value("h").toString().toDouble());
            }
        }
    }
    if (stream.hasError()) {
        QMessageBox::critical(this, tr("Error"), tr("The project cannot be loaded: ") + stream.errorString());
        return false;
    }

    if (startBox->value() < player->countFrames()) {
        player->seek(startBox->value());
    }
    return true;
}

QString MainWindow::projectDir() {
    QString pdir;
#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
    QDir dir("/usr/share/qgifer/");
    if (dir.exists())
        pdir = set->value("project_path", "/usr/share/qgifer/").toString();
    else
#endif
        pdir = set->value("project_path").toString();
    int ls = pdir.lastIndexOf("/");
    return ls != -1 ? pdir.left(ls) : "";
}

QString MainWindow::projectRelativePath(const QString &path) {
    QString pdir = projectDir();
    return QString(path).replace(pdir, "%project_path%");
}

void MainWindow::outputWidthChanged(int v) {
    if (!whRatioBox->isChecked() || whRatio <= 0) {
        return;
    }
    heightBox->disconnect();
    heightBox->setValue(ceil(v / whRatio));
    setChanged();
    connect(heightBox, SIGNAL(valueChanged(int)), this, SLOT(outputHeightChanged(int)));
}

void MainWindow::outputHeightChanged(int v) {
    if (!whRatioBox->isChecked() || whRatio <= 0) {
        return;
    }
    widthBox->disconnect();
    widthBox->setValue(ceil(v * whRatio));
    setChanged();
    connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(outputWidthChanged(int)));

}

void MainWindow::whRatioChanged(bool isChecked) {
    if (isChecked && player->isOpened()) {
        float outWHRatio = (float) widthBox->value() / (float) heightBox->value();
        Size originalSize = player->getOriginalSize();
        whRatio = (float) originalSize.width / (float) originalSize.height;
        if (outWHRatio < 1) {
            outputWidthChanged(widthBox->value());
        } else if (outWHRatio > 1) {
            outputHeightChanged(heightBox->value());

        }
    }
    setChanged();
}

void MainWindow::startChanged() {
    if (autoPaletteBox->isChecked()) {
        player->seek(startBox->value());
        updatePalette();
    }
    correctRange();
    multiSlider->setPosA(startBox->value(), false);
    setChanged();
}

void MainWindow::stopChanged() {
    correctRange();
    multiSlider->setPosB(stopBox->value(), false);
    setChanged();
}

void MainWindow::medianChanged(int m) {
    if (m && m % 2 == 0)
        medianSlider->setValue(m + 1);
    else {
        player->setMedianBlur(m);
        player->seek(player->getCurrentPos());
        medianLabel->setText(tr("Median blur (") + QString::number(m) + "):");
    }
    setChanged();
}

void MainWindow::varPaletteBoxChanged(int s) {
    if (!varPaletteBox->isEnabled())
        return;
    bool e = s == Qt::Checked;
    actionUpdatePalette->setEnabled(!e);
    actionOpenPalette->setEnabled(!e);
    actionSavePalette->setEnabled(!e);
    autoPaletteBox->setEnabled(!e);
    minDiffBox->setEnabled(e);
    paletteWidget->setEnabled(!e);
    setChanged();
}

void MainWindow::autoPaletteBoxChanged(int s) {
    if (!autoPaletteBox->isEnabled())
        return;
    bool e = s == Qt::Checked;
    actionUpdatePalette->setEnabled(!e);
    actionOpenPalette->setEnabled(!e);
    actionSavePalette->setEnabled(!e);
    varPaletteBox->setEnabled(!e);
    minDiffBox->setEnabled(!e && varPaletteBox->isEnabled() && varPaletteBox->isChecked());
    paletteWidget->setEnabled(!e);
    setChanged();
}


void MainWindow::openPalette() {

    QString path = QFileDialog::getOpenFileName(
            this, tr("Open QGifer palette file"),
            set->value("last_palette_dir", "").toString(),
            "QGifer Palette files (*.qgcm)");
    if (!path.isEmpty()) {
        if (!paletteWidget->fromFile(path))
            QMessageBox::critical(this, tr("Error"), tr("The palette file can not be loaded!"));
        else
            paletteBox->setValue(log(paletteWidget->getSize()) / log(2));

        setChanged();
    }

}

void MainWindow::savePalette() {
    QString path = QFileDialog::getSaveFileName(
            this, tr("Save QGifer palette file"),
            set->value("last_palette_dir", "").toString(),
            "QGifer Palette files (*.qgcm)");
    if (path.isEmpty())
        return;
    if (paletteWidget->toFile(path))
        set->setValue("last_palette_dir", path);
    else
        QMessageBox::critical(this, tr("Error"), tr("The palette file can not be saved!"));

}

void MainWindow::insertObject() {
    ObjectWidget *ow = new ObjectWidget(player);
    ow->setAttribute(Qt::WA_DeleteOnClose);
    ow->move(x() + width() * 0.3, y() + height() * 0.2);
    ow->setRange(startBox->value(), stopBox->value());
    ow->show();
    setChanged();
}

void MainWindow::insertText() {
    TextWidget *tw = new TextWidget(player);
    tw->setAttribute(Qt::WA_DeleteOnClose);
    tw->move(x() + width() * 0.3, y() + height() * 0.2);
    tw->setRange(startBox->value(), stopBox->value());
    tw->show();
    setChanged();
}

void MainWindow::showProperties(WorkspaceObject *o) {
    if (o->getTypeName() == "TextObject") {
        TextWidget *tw = new TextWidget(static_cast<TextObject *>(o), player);
        tw->setAttribute(Qt::WA_DeleteOnClose);
        tw->move(x() + width() * 0.3, y() + height() * 0.2);
        tw->show();
    }
    else {
        ObjectWidget *ow = new ObjectWidget(o, player);
        ow->setAttribute(Qt::WA_DeleteOnClose);
        ow->move(x() + width() * 0.3, y() + height() * 0.2);
        ow->show();
    }
    setChanged();
}

void MainWindow::newProject() {
    if (isChanged())
        switch (QMessageBox::question(
                this, tr("Question"), tr("The project has changed - do you want to save the changes?"), tr("Yes"),
                tr("No"), tr("Cancel"))) {
            case 0:
                saveProject();
                if (isChanged()) return;
                break;
            case 2:
                return;
                break;
        }

    player->close();
    paletteWidget->clear();
    player->getWorkspace()->getObjectsList()->clear();
    topSpin->setValue(10);
    rightSpin->setValue(10);
    bottomSpin->setValue(10);
    leftSpin->setValue(10);
    resetCorrection();
    correctionBox->setChecked(false);
    marginBox->setChecked(true);
    projectFile.clear();
    lock(true);
    setChanged(false);
    zoomSlider->setValue(100);
    player->showDefaultScreen();
}

void MainWindow::openProject() {
    if (isChanged()) {
        switch (QMessageBox::question(
                this, tr("Question"), tr("The project has changed - do you want to save the changes?"), tr("Yes"),
                tr("No"), tr("Cancel"))) {
            case 0:
                saveProject();
                if (isChanged()) return;
                break;
            case 2:
                return;
                break;
        }

    }
    QString path = QFileDialog::getOpenFileName(
            this, tr("Open project file"), projectDir(),
            "QGifer project (*.qgp);;All files (*)");
    if (!path.isEmpty()) if (!loadProject(path))
        QMessageBox::critical(this, tr("Error"), tr("Project was not entirely loaded."));

}

bool MainWindow::loadProject(const QString &file) {
    QFile f(file);
    if (f.open(QFile::Text | QFile::ReadOnly)) {
        set->setValue("project_path", file);
        if (projectFromXml(f.readAll())) {
            projectFile = file;
            setChanged(false);
            return true;
        }
    }
    else {
        QMessageBox::critical(this, tr("Error"), tr("Error reading file: ") + file);
    }
    return false;
}

void MainWindow::saveProject(const QString &file) {

    QString filepath = file;
    if (filepath.isEmpty()) {
        return;
    }
    setChanged(false);

    if (!filepath.endsWith(".qgp")) {
        filepath.append(".qgp");
    }
    QFile f(filepath);

    if (f.open(QFile::Text | QFile::WriteOnly)) {
        set->setValue("project_path", filepath);
        projectFile = filepath;
        QTextStream str(&f);
        str.setCodec("UTF-8");
        str.setAutoDetectUnicode(true);
        str << projectToXml();
        f.close();
    }
    else {
        QMessageBox::critical(this, tr("Error"), tr("Error writing file: ") + filepath);
    }
}

void MainWindow::saveProject() {
    QString file = projectFile.isEmpty() ? QFileDialog::getSaveFileName(
            this, tr("Save project file"), projectDir(),
            "QGifer project (*.qgp);;All files (*)") : projectFile;

    saveProject(file);
}

void MainWindow::saveProjectAs() {
    QString path = QFileDialog::getSaveFileName(
            this, tr("Save project file"), projectDir(),
            "QGifer project (*.qgp);;All files (*)");
    if (!path.isEmpty()) {
        set->setValue("project_path", path);
        saveProject(path);
        setChanged(false);
    }
}

void MainWindow::setChanged(bool c) {
    actionSaveProject->setEnabled(c);
    changed = c;
    setWindowTitle(projectFile.isEmpty() ? tr("QGifer - video-based GIF creator") : (projectFile +
                                                                                     (c ? tr(" [modified] ") : "") +
                                                                                     " - QGifer"));
}

void MainWindow::closeEvent(QCloseEvent *e) {
    if (isChanged()) {
        switch (QMessageBox::question(
                this, tr("Question"), tr("The project has changed - do you want to save the changes before exiting?"),
                tr("Yes"), tr("No"), tr("Cancel"))) {
            case 0:
                saveProject();
                if (!isChanged()) e->accept(); else e->ignore();
                break;
            case 1:
                e->accept();
                break;
            case 2:
                e->ignore();
        }

    }
    if (e->isAccepted()) {
        saveSettings();
        qApp->quit();
    }
}

void MainWindow::dockLevelChanged(bool top) {
    actionUndock->setText(top ? tr("Dock &toolbox") : tr("Undock &toolbox"));
    qDebug() << "top: " << top;
    if (top) {
        toolDock->resize(280, frameSize().height());
        QSize dsize = QApplication::desktop()->size();
        int space = 10;
        //miejsce z lewej
        if (x() > toolDock->width() + space)
            toolDock->move(x() - toolDock->width() - space, y());
            //miejsce z prawej
        else if (dsize.width() - (x() + frameSize().width()) > toolDock->width() + space)
            toolDock->move(x() + frameSize().width() + space, y());
    }

    if (correctionBox->isChecked())
        correctionChanged();
}

QString MainWindow::dataDir() {
    QDir dir("/usr/share/qgifer");
    return (dir.exists() ? dir.absolutePath() : qApp->applicationDirPath()) + "/";
}

void MainWindow::loadLanguages() {
    langs.clear();
    menuLanguage->clear();
    QStringList availableLanguages;
    QString qmpath = dataDir() + "locale/";
    QDirIterator qmIt(qmpath, QStringList() << "*.qm", QDir::Files);
    QAction *a = new QAction(tr("English"), menuLanguage);
    menuLanguage->addAction(a);
    langs.insert(a, "");
    while (qmIt.hasNext()) {
        qmIt.next();
        QFileInfo finfo = qmIt.fileInfo();
        QTranslator translator;
        translator.load(finfo.baseName(), qmpath);
        // qDebug() << "sciezka: " << qmpath;
        // qDebug() << "plik: " << finfo.baseName();
        QAction *a = new QAction(translator.translate("APPLICATION", "LANGUAGE_NAME"),
                                 menuLanguage);
        menuLanguage->addAction(a);
        langs.insert(a, finfo.baseName());
    }
}

void MainWindow::loadLanguage(const QString &basename, const QString &qmpath) {
    set->setValue("qmfile", basename);
    if (basename.isEmpty() || !QFile::exists(qmpath + "/" + basename + ".qm")) {
        if (translator)
            qApp->removeTranslator(translator);
        else
            return;
    }

    if (!translator)
        translator = new QTranslator();

    translator->load(basename, qmpath);
    qApp->installTranslator(translator);
    retranslateAll();
}

void MainWindow::languageChanged(QAction *a) {
    QString langfile = langs.value(a);
    qDebug() << "new language file: " << langfile;
    loadLanguage(langfile, dataDir() + "/locale");
}

void MainWindow::workspaceWheelRotated(int delta) {
    delta = delta / (delta > 0 ? delta : delta * -1);
    if (zoomSlider->isEnabled())
        zoomSlider->setValue(zoomSlider->value() + 8 * delta);
}

void MainWindow::renderDefaultText() {
    player->renderDefaultTextImage(
            tr("Press ") + actionOpenVideo->shortcut().toString() + tr(" to open a video"));
    if (locked)
        player->showDefaultScreen();
}

