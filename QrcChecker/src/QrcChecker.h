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

        bool operator==(const QString & fpath) const { return m_filePath == fpath; }
        /*! Index of QRC file in list of selected QRC files; -1 if resource is not yet referenced in a QRC file. */
		int		qrcIndex;
		/*! True, if contained in file system. */
		bool	m_exists;
		/*! Relative file path to QRC */
		QString	m_qrcFilePath;
		/*! Absolute file path to project base */
		QString	m_filePath;
	};

	void saveInput();
    void parseQrc(const QString & qrcFilePath);
    void parseCPP(const QString & cppFilePath);

    /*! Generates the wildcards string. */
    QString wildCards() const;


    Ui::QrcChecker                  *ui;
    std::vector<ResourceFileInfo>   m_resources;
};

extern const char * const ORGANIZATION;
extern const char * const PROGRAM_VERSION_NAME;

#endif // QrcChecker_H
