#ifndef PLOTWINDOW_CPP
#define PLOTWINDOW_CPP


#include "plotwindow.h"
#include <QWindow>
#include <QtWidgets>
#include <QList>
#include <QDebug>
#include <QInputDialog>
#include <QRegExp>

QList<qint32> userBaudRates;

PlotWindow::PlotWindow(void) {
	this->setWindowTitle("Line Plotter");
	this->setFixedWidth(300);
	this->setFixedHeight(403);

	QFont mainUiFont;
	mainUiFont.setFamily(QGuiApplication::font().family());
	mainUiFont.setPixelSize(13);

	QFont groupBoxFont;
	groupBoxFont.setFamily(QGuiApplication::font().family());
	groupBoxFont.setPixelSize(11);

	QFont smallUiFont;
	smallUiFont.setFamily(QGuiApplication::font().family());
	smallUiFont.setPixelSize(10);

	serialGroupBox = new QGroupBox(this);
	serialGroupBox->setGeometry(10, 10, 280, 130);
	serialGroupBox->setTitle("Serial Port Settings");
	serialGroupBox->setFont(groupBoxFont);
	

	serialSelectComboLabel = new QLabel(serialGroupBox);
	serialSelectComboLabel->setGeometry(12, 25, 200, 20);
	serialSelectComboLabel->setText("Serial Interface");
	serialSelectComboLabel->setFont(mainUiFont);

	serialSelectCombo = new QComboBox(serialGroupBox);
	serialSelectCombo->setGeometry(10, 42, 260, 28);	
	serialSelectCombo->addItem(" -");
	
	baudRateComboLabel = new QLabel(serialGroupBox);
	baudRateComboLabel->setGeometry(12, 75, 200, 20);
	baudRateComboLabel->setText("Baud Rate");
	baudRateComboLabel->setFont(mainUiFont);

	baudRateCombo = new QComboBox(serialGroupBox);
	baudRateCombo->setGeometry(10, 92, 260, 28);
	baudRateCombo->addItem(" -");
	baudRateCombo->setStyleSheet("text-align: right;");
	baudRateCombo->setEnabled(false);
	
	sensorGroupBox = new QGroupBox(this);
	sensorGroupBox->setGeometry(10, 150, 280, 145);
	sensorGroupBox->setTitle("Line Array Sensor Settings");
	sensorGroupBox->setFont(groupBoxFont);

	sensorSelectComboLabel = new QLabel(sensorGroupBox);
	sensorSelectComboLabel->setGeometry(12, 25, 200, 20);
	sensorSelectComboLabel->setText("Line Array Sensor");
	sensorSelectComboLabel->setFont(mainUiFont);

	sensorSelectCombo = new QComboBox(sensorGroupBox);
	sensorSelectCombo->setGeometry(10, 42, 260, 28);
	sensorSelectCombo->addItem(" -");

	sensorImageDecayCheckbox = new QCheckBox(sensorGroupBox);
	sensorImageDecayCheckbox->setGeometry(12, 85, 280, 28);
	sensorImageDecayCheckbox->setText("Show Previous Readings Decay");
	sensorImageDecayCheckbox->setStyleSheet("text-align: right;");
	sensorImageDecayCheckbox->setTristate(false);
	sensorImageDecayCheckbox->setFont(mainUiFont);

	imageDecayLineRadios = new QButtonGroup(sensorGroupBox);

	imageDecayLines4 = new QRadioButton(sensorGroupBox);
	imageDecayLines4->setGeometry(14, 114, 64, 28);
	imageDecayLines4->setText("4 Rows");
	imageDecayLines4->setFont(smallUiFont);

	imageDecayLines8 = new QRadioButton(sensorGroupBox);
	imageDecayLines8->setGeometry(77, 114, 64, 28);
	imageDecayLines8->setText("8 Rows");
	imageDecayLines8->setFont(smallUiFont);

	imageDecayLines12 = new QRadioButton(sensorGroupBox);
	imageDecayLines12->setGeometry(140, 114, 64, 28);
	imageDecayLines12->setText("12 Rows");
	imageDecayLines12->setFont(smallUiFont);

	imageDecayLines16 = new QRadioButton(sensorGroupBox);
	imageDecayLines16->setGeometry(203, 114, 64, 28);
	imageDecayLines16->setText("16 Rows");
	imageDecayLines16->setFont(smallUiFont);

	imageDecayLineRadios->addButton(imageDecayLines4);
	imageDecayLineRadios->addButton(imageDecayLines8);
	imageDecayLineRadios->addButton(imageDecayLines12);
	imageDecayLineRadios->addButton(imageDecayLines16);

	imageDecayLines4->setChecked(true);

	imageDecayLines4->setEnabled(false);
	imageDecayLines8->setEnabled(false);
	imageDecayLines12->setEnabled(false);
	imageDecayLines16->setEnabled(false);


	buttonGroupBox = new QGroupBox(this);
	buttonGroupBox->setGeometry(10, 330, 280, 58);
	buttonGroupBox->setTitle("");
	buttonGroupBox->setFont(groupBoxFont);

	mainButtonGroup = new QButtonGroup(buttonGroupBox);

	exitButton = new QPushButton(buttonGroupBox);
	exitButton->setGeometry(5, 7, 135, 48);
	exitButton->setText("&Exit");

	actionButton = new QPushButton(buttonGroupBox);
	actionButton->setGeometry(140, 7, 135, 48);
	actionButton->setText("&Visualize");

	mainButtonGroup->addButton(exitButton);
	mainButtonGroup->addButton(actionButton);

	horizontalLine = new QFrame(sensorGroupBox);
	horizontalLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	horizontalLine->setGeometry(5, 79, 271, 3);

	this->populateSerialPortCombo();
	this->populateSensorSelectCombo();
	connect(serialSelectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(serialSelectChanged(int)));
	connect(baudRateCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(baudRateChanged(int)));
	connect(sensorImageDecayCheckbox, SIGNAL(stateChanged(int)), this, SLOT(imageDecayChanged(int)));
	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(actionButton, SIGNAL(clicked()), this, SLOT(startVisualizing()));

	renderWindow = new RenderWindow(this);

	connect(renderWindow, SIGNAL(closeMyself()), this, SLOT(abortVisualizing()));
}


void PlotWindow::populateSerialPortCombo(void) {

	Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
		serialSelectCombo->addItem(port.systemLocation());
	}

}


void PlotWindow::populateSensorSelectCombo(void) {
	sensorSelectCombo->addItem("TAOS/ams TSL1401 - (128 Pixel)");
	sensorSelectCombo->addItem("TAOS/ams TSL1402 - (256 Pixel)");
	sensorSelectCombo->addItem("TAOS/ams TSL1406 - (768 Pixel)");
    sensorSelectCombo->addItem("TAOS/ams TSL1412 - (1536 Pixel)");
    sensorSelectCombo->addItem("TAOS/ams TSL201 - (64 Pixel)");
    sensorSelectCombo->addItem("iC-Haus iC-LF1401 - (128 Pixel)");
}


void PlotWindow::serialSelectChanged(int serialSelectComboIndex) {
	if (serialSelectComboIndex == 0) { // "-" selected
		baudRateCombo->clear();
		baudRateCombo->addItem(" -");
		baudRateCombo->setEnabled(false);
		return;
	} else {
		qDebug() << serialSelectCombo->itemText(serialSelectComboIndex);
		Q_FOREACH(qint32 baudRate, QSerialPortInfo::standardBaudRates()) {
			if (baudRate >= 1200) {
				baudRateCombo->addItem(QString("%1").arg(baudRate));
			}
		}
		baudRateCombo->insertSeparator(baudRateCombo->count());
		baudRateCombo->addItem("Custom...");

		if (!userBaudRates.isEmpty()) {
			Q_FOREACH(qint32 customBaudRate, userBaudRates) {
					for (int i = 1; i <= (baudRateCombo->count()-2); i++) {
						int compareBaudRate = 1200;
						if (baudRateCombo->itemText(i).startsWith("* ")) {
							compareBaudRate = QString(baudRateCombo->itemText(i).remove(0,2)).toInt();
						} else {
							compareBaudRate = QString(baudRateCombo->itemText(i)).toInt();
						}
						if ((i == baudRateCombo->count()-2) || (customBaudRate < compareBaudRate)) {
							baudRateCombo->insertItem(i, QString("* %1").arg(customBaudRate));
							break;
						}
			
					}
			}
		}
		baudRateCombo->setEnabled(true);
	}
	return;
}

void PlotWindow::baudRateChanged(int baudRateComboIndex) {
	if (baudRateCombo->itemText(baudRateComboIndex) == "Custom...") {
		baudRateCombo->blockSignals(true);
		bool ok;
		int newIndex = 0;
		int customBaudRate = QInputDialog::getInt(this, "Custom Baud Rate", "Please specify a custom Baud Rate\n\n(Warning: This is not supported on all Platforms\nand may cause serious System stability issues!)\n\n", 1200, 1200, 2147483647, 1, &ok);
		if (ok) {
			if (!userBaudRates.isEmpty()) {
				Q_FOREACH(qint32 tempUserBaudRate, userBaudRates) {
					if (tempUserBaudRate == customBaudRate) { return; }
				}
			}
			userBaudRates.append(customBaudRate);
			for (int i = 1; i <= (baudRateCombo->count()-2); i++) {
				int compareBaudRate = 1200;
				if (baudRateCombo->itemText(i).startsWith("* ")) {
					compareBaudRate = QString(baudRateCombo->itemText(i).remove(0,2)).toInt();
				} else {
					compareBaudRate = QString(baudRateCombo->itemText(i)).toInt();
				}
				if ((i == baudRateCombo->count()-2) || (customBaudRate < compareBaudRate)) {
					baudRateCombo->insertItem(i, QString("* %1").arg(customBaudRate));
					newIndex = i;
					break;
				}
			
			}
		}
	baudRateCombo->setCurrentIndex(newIndex);
	baudRateCombo->blockSignals(false);
	}
	return;
}

void PlotWindow::imageDecayChanged(int imageDecayState) {
	if(imageDecayState == Qt::Checked) {
		imageDecayLines4->setEnabled(true);
		imageDecayLines8->setEnabled(true);
		imageDecayLines12->setEnabled(true);
		imageDecayLines16->setEnabled(true);
	} else {
		imageDecayLines4->setEnabled(false);
		imageDecayLines8->setEnabled(false);
		imageDecayLines12->setEnabled(false);
		imageDecayLines16->setEnabled(false);
	}
}

void PlotWindow::getSerialPortInfo(QString name) {
	
}

void PlotWindow::startVisualizing(void) {

	if (sensorSelectCombo->currentText() == " -") { return; }
	if (serialSelectCombo->currentText() == " -") { return; }
	if ((baudRateCombo->currentText() == " -") || (baudRateCombo->currentText() == "Custom...")) { return; }

	actionButton->blockSignals(true);
	actionButton->setEnabled(false);

	int pixelSize = 0;

	QRegExp rx("^.+\\s-\\s\\(\\s?([0-9]+)\\sPixel\\s?\\)$");

	rx.indexIn(sensorSelectCombo->currentText());

	pixelSize = QString(rx.capturedTexts()[1]).toInt();

	QString windowTitle = QString(sensorSelectCombo->currentText().split(" - ")[0]);

	renderWindow->prepare(pixelSize, windowTitle);
	renderWindow->show();
}


void PlotWindow::abortVisualizing(void) {
	actionButton->blockSignals(false);
	actionButton->setEnabled(true);
}

RenderWindow::RenderWindow(QObject *parent) {
	imageFrame = new QFrame(this);
	imageFrame->setGeometry(8, 8, 72, 10);
	imageFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
	fpsLabel = new QLabel(this);
	fpsLabel->setGeometry(8, 26, 42, 20);
	fpsLabel->setText("FPS:");
	closeButton = new QPushButton(this);
	closeButton->setGeometry(30, 24, 48, 24);
	closeButton->setText("&Close");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));
}

void RenderWindow::closeEvent(QCloseEvent *event) {
    this->emitClosingSignal();
    this->hide();
}

void RenderWindow::hideEvent(QHideEvent *event) {
	this->closeEvent((QCloseEvent *) event);
}

void RenderWindow::emitClosingSignal(void) {
		emit closeMyself();
}

void RenderWindow::prepare(int PixelSize, QString windowTitle) {
	int fLeft = imageFrame->geometry().left();
	int fTop = imageFrame->geometry().top();
	int fWidth = PixelSize + 8;
	int fHeight = imageFrame->geometry().height();
	imageFrame->setGeometry( fLeft, fTop, fWidth, fHeight );
	this->setFixedWidth(imageFrame->geometry().width()+16);
	this->setFixedHeight(fHeight+48);
	this->setWindowTitle(windowTitle);
	if (PixelSize >= 128) {
		closeButton->move(fWidth-41, closeButton->y());
	} else {
		closeButton->setGeometry( 8, closeButton->y()+22, fWidth, 24 );
		this->setFixedHeight(this->height()+24);
	} 	
}



#endif // PLOTWINDOW_CPP
