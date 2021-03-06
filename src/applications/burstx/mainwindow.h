/* -*- c++ -*- */
/*
 * Gqrx SDR: Software defined radio receiver powered by GNU Radio and Qt
 *           http://gqrx.dk/
 *
 * Copyright 2011-2014 Alexandru Csete OZ9AEC.
 *
 * Gqrx is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gqrx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gqrx; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColor>
#include <QMainWindow>
#include <QPointer>
#include <QSettings>
#include <QString>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QSvgWidget>

#include "qtgui/dockrxopt.h"
#include "qtgui/dockburst.h"
#include "qtgui/dockinputctl.h"
#include "qtgui/dockfft.h"
#include "qtgui/dockbookmarks.h"
#include "qtgui/iq_tool.h"


// see https://bugreports.qt-project.org/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include "applications/burstx/receiver.h"
#endif

namespace Ui {
    class MainWindow;  /*! The main window UI */
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString cfgfile, bool edit_conf, QWidget *parent = 0);
    ~MainWindow();

    bool loadConfig(const QString cfgfile, bool check_crash, bool restore_mainwindow);
    bool saveConfig(const QString cfgfile);
    void storeSession();

    bool configOk; /*!< Main app uses this flag to know whether we should abort or continue. */

public slots:
    void setNewFrequency(qint64 rx_freq);

private:
    Ui::MainWindow *ui;

    QPointer<QSettings> m_settings;  /*!< Application wide settings. */
    QString             m_cfg_dir;   /*!< Default config dir, e.g. XDG_CONFIG_HOME. */
    QString             m_last_dir;

    qint64 d_lnb_lo;  /* LNB LO in Hz. */
    qint64 d_hw_freq;
    qint64 d_hw_freq_start;
    qint64 d_hw_freq_stop;

    std::complex<float>* d_fftData;
    float          *d_realFftData;
    float          *d_iirFftData;
    float          *d_pwrFftData;
    float           d_fftAvg;      /*!< FFT averaging parameter set by user (not the true gain). */

    /* dock widgets */
    DockRxOpt      *uiDockRxOpt;
    DockBurst      *uiDockBurst;
    DockInputCtl   *uiDockInputCtl;
    DockFft        *uiDockFft;
    DockBookmarks  *uiDockBookmarks;

    CIqTool        *iq_tool;

    QTimer   *dec_timer;
    QTimer   *meter_timer;
    QTimer   *iq_fft_timer;
    QTimer   *burst_fft_timer;

    receiver *rx;

    std::map<QString, QVariant> devList;

    // dummy widget to enforce linking to QtSvg
    QSvgWidget      *qsvg_dummy;

private:
    void updateHWFrequencyRange(bool ignore_limits);
    void updateFrequencyRange();
    void updateGainStages(bool read_from_device);
    void showSimpleTextFile(const QString &resource_path,
                            const QString &window_title);

private slots:
    /* rf */
    void setLnbLo(double freq_mhz);
    void setAntenna(const QString antenna);

    /* baseband receiver */
    void setFilterOffset(qint64 freq_hz);
    void setGain(QString name, double gain);
    void setAutoGain(bool enabled);
    void setFreqCorr(double ppm);
    void setIqSwap(bool reversed);
    void setDcCancel(bool enabled);
    void setIqBalance(bool enabled);
    void setIgnoreLimits(bool ignore_limits);
    void setFreqCtrlReset(bool enabled);
    void selectDemod(QString demod);
    void selectDemod(int index);
    void setFmMaxdev(float max_dev);
    void setFmEmph(double tau);
    void setAgcHang(bool use_hang);
    void setAgcThreshold(int threshold);
    void setAgcSlope(int factor);
    void setAgcDecay(int msec);
    void setAgcGain(int gain);
    void setNoiseBlanker(int nbid, bool on, float threshold);
    void setSqlLevel(double level_db);
    double setSqlLevelAuto();
    void setPassband(int bandwidth);

    /* dock burst */
    void startBurstRec(const QString filename);
    void stopBurstRec();
    void setSquelchThreshold(double threshold);

/*    void startAudioStream(const QString udp_host, int udp_port);
    void stopAudioStreaming();
*/

    /* I/Q playback and recording*/
    void startIqPlayback(const QString filename, float samprate);
    void stopIqPlayback();
    void seekIqFile(qint64 seek_pos);

    /* FFT settings */
    void setIqFftSize(int size);
    void setIqFftRate(int fps);
    void setIqFftSplit(int pct_wf);
    void setIqFftAvg(float avg);
    void setBurstFftRate(int fps);
    void setFftColor(const QColor color);
    void setFftFill(bool enable);
    void setPeakDetection(bool enabled);
    void setFftPeakHold(bool enable);
    void setWfTimeSpan(quint64 span_ms);

    /* FFT plot */
    void on_plotter_newDemodFreq(qint64 freq, qint64 delta);   /*! New demod freq (aka. filter offset). */
    void on_plotter_newFilterFreq(int low, int high);    /*! New filter width */
    void on_plotter_newCenterFreq(qint64 f);


    /* Bookmarks */
    void onBookmarkActivated(qint64 freq, QString demod, int bandwidth);

    /* menu and toolbar actions */
    void on_actionDSP_triggered(bool checked);
    int  on_actionIoConfig_triggered();
    void on_actionLoadSettings_triggered();
    void on_actionSaveSettings_triggered();
    void on_actionSaveWaterfall_triggered();
    void on_actionFullScreen_triggered(bool checked);
    void on_actionUserGroup_triggered();
    void on_actionNews_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_actionAddBookmark_triggered();


    /* window close signals */
    int  firstTimeConfig();

    /* cyclic processing */
    void decoderTimeout();
    void meterTimeout();
    void iqFftTimeout();
    void burstFftTimeout();
};

#endif // MAINWINDOW_H
