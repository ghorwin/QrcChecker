#include "QrcChecker.h"
#include "ui_QrcChecker.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QSettings>

const char * const ORGANIZATION = "IBK";
const char * const PROGRAM_VERSION_NAME = "QrcChecker 1.0";


QrcChecker::QrcChecker(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::QrcChecker)
{
	ui->setupUi(this);

	setWindowTitle(PROGRAM_VERSION_NAME);

	// restore input from settings
	QSettings settings(ORGANIZATION, PROGRAM_VERSION_NAME);
	QString baseDir = settings.value("BaseDirectory", QString()).toString();
	QString wildCards = settings.value("FileTypeWildCards", QString()).toString();
	ui->lineEditBaseDirectory->setText(baseDir);
	ui->checkBoxJPGFiles->setChecked( wildCards.contains("*.jpg"));
	ui->checkBoxPNGFiles->setChecked( wildCards.contains("*.png"));
	int i = wildCards.indexOf("*.jpg");
	if (i != -1)
		wildCards = wildCards.remove(i, 5);
	i = wildCards.indexOf("*.png");
	if (i != -1)
		wildCards = wildCards.remove(i, 5);
	ui->lineEditResourceFileTypes->setText(wildCards);

	QStringList qrcFiles = settings.value("QRCFiles", QStringList()).toStringList();
	ui->tableWidgetQrcFiles->setRowCount(qrcFiles.count());
	for (int j=0; j<qrcFiles.size(); ++j) {
		QString qrc = qrcFiles[j];
		QTableWidgetItem * w = new QTableWidgetItem(qrc);
		w->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->tableWidgetQrcFiles->setItem(j,0,w);
	}
}


QrcChecker::~QrcChecker() {
	delete ui;
}


void QrcChecker::on_pushButtonQuit_clicked() {
	saveInput();
	close();
}


void QrcChecker::on_pushButtonSelectBaseDir_clicked() {
	QString baseDir = QFileDialog::getExistingDirectory(this, tr("Select project base directory"), ui->lineEditBaseDirectory->text());
	if (baseDir.isEmpty())
		return;
	ui->lineEditBaseDirectory->setText(baseDir);
}


void QrcChecker::on_toolButtonAddQrc_clicked() {
	QString qrcFile = QFileDialog::getOpenFileName(this, tr("Select qrc file within your project directory"),
												   ui->lineEditBaseDirectory->text(), tr("QRC-files (*.qrc)"));
	if (qrcFile.isEmpty())
		return;
	// compose relative path to base directory
	QDir baseDir(ui->lineEditBaseDirectory->text());
	QString relPath = baseDir.relativeFilePath(qrcFile);
	int r = ui->tableWidgetQrcFiles->rowCount();
	ui->tableWidgetQrcFiles->setRowCount(r+1);
	QTableWidgetItem * w = new QTableWidgetItem(relPath);
	ui->tableWidgetQrcFiles->blockSignals(true);
	ui->tableWidgetQrcFiles->setItem(r,0,w);
	// select last item
	ui->tableWidgetQrcFiles->selectRow(r);
	ui->tableWidgetQrcFiles->blockSignals(false);
	on_tableWidgetQrcFiles_itemSelectionChanged();
}


void QrcChecker::on_tableWidgetQrcFiles_itemSelectionChanged() {
	QItemSelection sel = ui->tableWidgetQrcFiles->selectionModel()->selection();
	if (sel.isEmpty())
		ui->toolButtonRemoveQrc->setEnabled(false);
	else
		ui->toolButtonRemoveQrc->setEnabled(true);
}


void QrcChecker::on_toolButtonRemoveQrc_clicked() {
	QItemSelection sel = ui->tableWidgetQrcFiles->selectionModel()->selection();
	Q_ASSERT(!sel.isEmpty());

	ui->tableWidgetQrcFiles->blockSignals(true);
	int i=sel.first().top();
	ui->tableWidgetQrcFiles->removeRow(i);
	if (i == ui->tableWidgetQrcFiles->rowCount())
		--i;
	if (i != -1)
		ui->tableWidgetQrcFiles->removeRow(i);
	ui->tableWidgetQrcFiles->blockSignals(false);
	on_tableWidgetQrcFiles_itemSelectionChanged();
}


void QrcChecker::on_pushButtonScan_clicked() {
	saveInput();

}


void QrcChecker::saveInput() {

}
