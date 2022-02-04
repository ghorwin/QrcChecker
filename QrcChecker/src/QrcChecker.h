#ifndef QrcChecker_H
#define QrcChecker_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class QrcChecker; }
QT_END_NAMESPACE

class QrcChecker : public QDialog {
	Q_OBJECT

public:
	QrcChecker(QWidget *parent = nullptr);
	~QrcChecker();

private slots:
	void on_pushButtonQuit_clicked();

	void on_pushButtonSelectBaseDir_clicked();

	void on_toolButtonAddQrc_clicked();

	void on_tableWidgetQrcFiles_itemSelectionChanged();

	void on_toolButtonRemoveQrc_clicked();

	void on_pushButtonScan_clicked();

    void on_lineEditBaseDirectory_editingFinished();

private:
	/*! Holds information about a referenced resource. */
	struct ResourceFileInfo {
		/*! Index of QRC file in list; -1 if not in QRC file. */
		int		qrcIndex;
		/*! True, if contained in file system. */
		bool	m_exists;
		/*! Relative file path to QRC */
		QString	m_qrcFilePath;
		/*! Absolute file path to project base */
		QString	m_filePath;
	};

	void saveInput();

	Ui::QrcChecker *ui;
};

extern const char * const ORGANIZATION;
extern const char * const PROGRAM_VERSION_NAME;

#endif // QrcChecker_H
