#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QWidget>
#include <QWindow>
#include <QMainWindow>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QObject>
#include <QList>
#include <QCheckbox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QFrame>
#include <QFont>
#include <QEvent>



class RenderWindow : public QWidget
{
  Q_OBJECT
  public:
  	explicit RenderWindow(QObject *parent);
  signals:
  	void closeMyself(void);
  private:
  	QFrame *imageFrame;
  	QLabel *fpsLabel;
  	QPushButton *closeButton;
  protected slots:
    void closeEvent(QCloseEvent *event);
    void hideEvent(QHideEvent *event);
  private slots:
  	void emitClosingSignal(void);
  public slots:
  	void prepare(int PixelSize, QString windowTitle);
};


class PlotWindow : public QMainWindow
{
  Q_OBJECT

public:
	PlotWindow(void);
protected:
	RenderWindow *renderWindow;
private:
	QGroupBox *serialGroupBox;
	QLabel *serialSelectComboLabel;
	QComboBox *serialSelectCombo;
	QLabel *baudRateComboLabel;
	QComboBox *baudRateCombo;
	QGroupBox *sensorGroupBox;
	QLabel *sensorSelectComboLabel;
	QComboBox *sensorSelectCombo;
	QCheckBox *sensorImageDecayCheckbox;
	QButtonGroup *imageDecayLineRadios;
	QRadioButton *imageDecayLines4;
	QRadioButton *imageDecayLines8;
	QRadioButton *imageDecayLines12;
	QRadioButton *imageDecayLines16;
	QGroupBox *buttonGroupBox;
	QButtonGroup *mainButtonGroup;
	QPushButton *exitButton;
	QPushButton *actionButton;
	QFont mainUiFont;
	QFont groupBoxFont;
	QFont smallUiFont;
	QFrame *horizontalLine;

private slots:
	void getSerialPortInfo(QString name);
	void populateSerialPortCombo(void);
	void populateSensorSelectCombo(void);
	void serialSelectChanged(int serialSelectComboIndex);
	void baudRateChanged(int baudRateComboIndex);
	void imageDecayChanged(int imageDecayState);
	void startVisualizing(void);
	void abortVisualizing(void);
};


#endif // PLOTWINDOW_H
