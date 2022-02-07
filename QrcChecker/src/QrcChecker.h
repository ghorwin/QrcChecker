#ifndef QrcChecker_H
#define QrcChecker_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class QrcChecker; }
class QXmlStreamReader;
QT_END_NAMESPACE

/*! The main widget of the QrcChecker application. */
class QrcChecker : public QWidget {
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
		/*! Index of respectiv QRC file in list of selected QRC files; -1 if resource is not yet referenced in a QRC file. */
		int		m_qrcIndex = -1;
		/*! True, if contained in file system. */
		bool	m_exists = false;
		/*! The QRC Path like ':/img/myPicture.png' as it is used in the cpp file; may include Prefix */
		QString	m_qrcPath;
		/*! Contains the filename of the first cpp file that contains this resource reference.
			If this is empty, the resource file is nowhere used
		*/
		QString m_cppFile;
		/*! Absolute file path to project base */
		QString	m_filePath;
	};

	void saveInput();
	void parseQrc(const QString & qrcFilePath);
	void parseCPP(const QString & cppFilePath);
	void parseUI(const QString & uiFilePath);
	bool parseResource(const QString & qrcFileDirectory, QXmlStreamReader & xml);

	/*! Generates the wildcards string. */
	QString wildCards() const;


	Ui::QrcChecker                  *ui;
	std::vector<ResourceFileInfo>   m_resources;
};

extern const char * const ORGANIZATION;
extern const char * const PROGRAM_VERSION_NAME;

#endif // QrcChecker_H
