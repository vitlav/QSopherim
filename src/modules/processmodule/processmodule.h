#ifndef __PROCESSMODULE__H
#define __PROCESSMODULE__H
#include <QObject>

#include "biblequote.h"
#include "biblequotedictmodule/biblequotedictmodule.h"
#include <QString>


class ProcessModule: public QObject
{
    Q_OBJECT

public:
    explicit ProcessModule(QString pathToModule, int typeModule);
    explicit ProcessModule();

    ~ProcessModule();

    bool processing(QString pathToModule, int type);
signals:
    void SIGNAL_ProcessModuleOk();
    void SIGNAL_ProcessDictOk();
    void SIGNAL_ProcessCommentsOk();
    void SIGNAL_ProcessApocryphaOk();

private slots:
    void createFolderForModule(QString shortname);

private:
    BibleQuoteModule* m_BibleQuote;
    BibleQuoteModule* m_BibleQuoteComments;
    BibleQuoteModule* m_BibleQuoteBook;
    BibleQuoteModule* m_BibleQuoteApocrypha;
    BibleQuoteDictModule* m_BibleQuoteDictModule;
    void createConnects();
    void init();
    QString p_pathToModule;
};


#endif // __PROCESSMODULE__H
