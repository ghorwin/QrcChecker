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

private:
	void saveInput();

	Ui::QrcChecker *ui;
};

extern const char * const ORGANIZATION;
extern const char * const PROGRAM_VERSION_NAME;

#endif // QrcChecker_H
