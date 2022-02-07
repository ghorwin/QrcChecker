#include "QrcChecker.h"
#include "ui_QrcChecker.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QSettings>
#include <QXmlStreamReader>
#include <QDirIterator>
#include <QFontMetrics>

const char * const ORGANIZATION = "IBK";
const char * const PROGRAM_VERSION_NAME = "QrcChecker 1.0";


QrcChecker::QrcChecker(QWidget *parent)
	: QWidget(parent), ui(new Ui::QrcChecker)
{
	ui->setupUi(this);

	setWindowTitle(PROGRAM_VERSION_NAME);

	// restore input from settings
	QSettings settings(ORGANIZATION, PROGRAM_VERSION_NAME);
	QString baseDir = settings.value("BaseDirectory", QString()).toString();
	QString wcs = settings.value("FileTypeWildCards", QString()).toString();
	QStringList wc = wcs.split(";", QString::SkipEmptyParts);
	ui->lineEditBaseDirectory->setText(baseDir);
	ui->checkBoxJPGFiles->setChecked( wc.contains("*.jpg"));
	ui->checkBoxPNGFiles->setChecked( wc.contains("*.png"));
	wc.removeAll("*.jpg");
	wc.removeAll("*.png");
	ui->lineEditResourceFileTypes->setText(wc.join(";"));

	QStringList qrcFiles = settings.value("QRCFiles", QStringList()).toStringList();
	ui->tableWidgetQrcFiles->setRowCount(qrcFiles.count());
	for (int j=0; j<qrcFiles.size(); ++j) {
		QString qrc = qrcFiles[j];
		QTableWidgetItem * w = new QTableWidgetItem(qrc);
		QString absPath = QDir(baseDir).absoluteFilePath(qrc);
		w->setData(Qt::UserRole, absPath);
		w->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->tableWidgetQrcFiles->setItem(j,0,w);
	}

	// customize tables
	QFont f;
	f.setPointSizeF( f.pointSizeF() * 0.8);
	QFontMetrics fm(f);
	int rowHeight = fm.lineSpacing();
	ui->tableWidget->setFont(f);
	ui->tableWidget->verticalHeader()->setDefaultSectionSize(rowHeight);
	ui->tableWidget->horizontalHeaderItem(0)->setToolTip(tr("True, if this file is referenced in a QRC file"));
	ui->tableWidget->horizontalHeaderItem(1)->setToolTip(tr("The QRC resource path in form ':/<prefix>/filename.png'"));
	ui->tableWidget->horizontalHeaderItem(2)->setToolTip(tr("File path on disc"));
	ui->tableWidget->horizontalHeaderItem(3)->setToolTip(tr("True, if the respective file exists"));
	ui->tableWidget->horizontalHeaderItem(4)->setToolTip(tr("Source code file that references this resource"));
	ui->tableWidget->resizeColumnsToContents();

	ui->tableWidgetQrcFiles->setFont(f);
	ui->tableWidgetQrcFiles->verticalHeader()->setDefaultSectionSize(rowHeight);

	ui->pushButtonRemoveUnusedFromQrc->setEnabled(false);
	ui->pushButtonRemoveUnusedFromFileSystem->setEnabled(false);
	ui->toolButtonRemoveQrc->setEnabled(false);
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
	w->setData(Qt::UserRole, qrcFile); // store absolute file path
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

	// start by parsing qrc files
	QStringList referencedFiles;
	m_resources.clear();
	for (int j=0; j<ui->tableWidgetQrcFiles->rowCount(); ++j) {
		QString qrcFile = ui->tableWidgetQrcFiles->item(j,0)->data(Qt::UserRole).toString();

		// parse QRC file and add files to m_resources
		parseQrc(qrcFile);
	}

	// now recursively scan the directory structure for wanted resources and c++/cpp files
	QString baseDirPath(ui->lineEditBaseDirectory->text());
	QStringList wildcards = wildCards().split(";");
	// add source files, these will be parsed separately
	wildcards << "*.cpp" << "*.cxx" << "*.ui";
	QDirIterator it(baseDirPath, wildcards, QDir::Files, QDirIterator::Subdirectories);
	QStringList srcFiles;
	QStringList uiFiles;
	while (it.hasNext()) {
		QString resFilePath = it.next();
		if (resFilePath.endsWith(".cpp") || resFilePath.endsWith(".cxx")) {
			srcFiles.push_back(resFilePath);
			continue;
		}
		if (resFilePath.endsWith(".ui")) {
			uiFiles.push_back(resFilePath);
			continue;
		}
		// regular resource file
		auto resIt = std::find(m_resources.begin(), m_resources.end(), resFilePath);
		if (resIt == m_resources.end()) { // not found?
			ResourceFileInfo newRes;
			newRes.m_exists = true;
			newRes.m_qrcIndex = -1;
			newRes.m_filePath = resFilePath;
			m_resources.push_back(newRes);
		}
		else {
			// already referenced in QRC? Mark as existing on disk
			resIt->m_exists = true;
		}
	}

	// scan CPP files and add/update info in m_resources
	for (QString s : srcFiles)
		parseCPP(s);

	// now populate the table
	QDir baseDir(ui->lineEditBaseDirectory->text());
	ui->tableWidget->setRowCount(m_resources.size());
	for (int i=0; i<(int)m_resources.size(); ++i) {
		const ResourceFileInfo & resInfo = m_resources[i];
		QTableWidgetItem * item = new QTableWidgetItem();
		if (resInfo.m_qrcIndex == -1)
			item->setCheckState(Qt::Unchecked);
		else
			item->setCheckState(Qt::Checked);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->tableWidget->setItem(i, 0, item);

		item = new QTableWidgetItem(resInfo.m_qrcPath);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->tableWidget->setItem(i, 1, item);

		item = new QTableWidgetItem();
		if (!resInfo.m_exists)
			item->setCheckState(Qt::Unchecked);
		else
			item->setCheckState(Qt::Checked);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->tableWidget->setItem(i, 2, item);

		// generate relative file path to base Dir
		QString relPath = baseDir.relativeFilePath(resInfo.m_filePath);
		if (resInfo.m_filePath.isEmpty())
			relPath.clear();
		item = new QTableWidgetItem(relPath);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->tableWidget->setItem(i, 3, item);

		// generate relative file path to base Dir
		relPath = baseDir.relativeFilePath(resInfo.m_cppFile);
		if (resInfo.m_cppFile.isEmpty())
			relPath.clear();
		item = new QTableWidgetItem(relPath);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->tableWidget->setItem(i, 4, item);
	}
	ui->tableWidget->resizeColumnsToContents();
}


void QrcChecker::saveInput() {
	QSettings settings(ORGANIZATION, PROGRAM_VERSION_NAME);
	settings.setValue("BaseDirectory", ui->lineEditBaseDirectory->text());
	settings.setValue("FileTypeWildCards", wildCards());

	QStringList qrcFiles;
	for (int j=0; j<ui->tableWidgetQrcFiles->rowCount(); ++j)
		qrcFiles.append( ui->tableWidgetQrcFiles->item(j,0)->text() );

	settings.setValue("QRCFiles", qrcFiles);
}


void QrcChecker::parseQrc(const QString &qrcFilePath) {
	QFile xmlFile(qrcFilePath);
	if (!xmlFile.open(QFile::ReadOnly))
		return; // skip invalid QRC files
	QXmlStreamReader xml(&xmlFile);
	while (!xml.atEnd()) {
		xml.readNext();
		//
	}
	if (xml.hasError()) {
		QMessageBox::critical(this, QString(), tr("Error reading QRC file '%1").arg(qrcFilePath));
	}

}


void QrcChecker::parseCPP(const QString &cppFilePath) {
	// read CPP file line by line and extract strings starting with '":/'
	QFile f(cppFilePath);
	if (!f.open(QFile::ReadOnly))
		return;
	QStringList lines = QString(f.readAll()).split("\n");
	for (int i = 0; i<lines.count(); ++i) {
		QString l = lines[i];
		int p = l.indexOf("\":/");
		if (p != -1) {
			// find closing "
			int p2 = l.indexOf('\"', p+1);
			if (p2 != -1 && p2-p>3) {
				QString qrcPath = l.mid(p+1, p2-p-1);
				// lookup qrcPath in resources
				std::vector<ResourceFileInfo>::iterator it = m_resources.begin();
				for (; it != m_resources.end(); ++it) {
					if (it->m_qrcPath == qrcPath) {
						it->m_referenced = true;
						if (it->m_cppFile.isEmpty())
							it->m_cppFile = cppFilePath + QString(":%1").arg(i+1);
						break;
					}

				}
				if (it == m_resources.end()) {
					// found a reference without matching QRC entry
					ResourceFileInfo res;
					res.m_exists = false;
					res.m_qrcIndex = -1;
					res.m_qrcPath = qrcPath;
					res.m_referenced = true;
					res.m_cppFile = cppFilePath + QString(":%1").arg(i+1);
					m_resources.push_back(res);
				}
			}

		}
	}
}


QString QrcChecker::wildCards() const {
	QString wildCards = ui->lineEditResourceFileTypes->text().trimmed().toLower();
	if (ui->checkBoxJPGFiles->isChecked() && !wildCards.contains("*.jpg")) {
		if (!wildCards.isEmpty())
			wildCards += ";";
		wildCards += "*.jpg";
	}
	if (ui->checkBoxPNGFiles->isChecked() && !wildCards.contains("*.png")) {
		if (!wildCards.isEmpty())
			wildCards += ";";
		wildCards += "*.png";
	}
	return wildCards;
}



void QrcChecker::on_lineEditBaseDirectory_editingFinished() {
	// update relative file paths of qrc files
	for (int j=0; j<ui->tableWidgetQrcFiles->rowCount(); ++j) {
		QTableWidgetItem * w = ui->tableWidgetQrcFiles->item(j,0);
		QString qrcFile = w->data(Qt::UserRole).toString();
		QDir baseDir(ui->lineEditBaseDirectory->text());
		QString relPath = baseDir.relativeFilePath(qrcFile);
		w->setText(relPath);
	}
}
