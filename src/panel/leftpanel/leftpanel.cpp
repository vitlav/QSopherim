#include "leftpanel.h"
#include "ui_leftpanel.h"
#include "debughelper.h"
#include "projectqmodulelist.h"
#include "moduleviewer.h"

#include "config.h"


#include <QStandardItemModel>
#include <QStringListModel>



LeftPanel::LeftPanel(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LeftPanel)
{
    ui->setupUi(this);
    init();
}
//------------------------------------------------------------------------------
LeftPanel::~LeftPanel()
{
    delete ui;
}
//------------------------------------------------------------------------------
void LeftPanel::refreshList(ProjectQModuleList* list)
{

//    this->modelModules = new QStandardItemModel(moduleList->getSize(), 1, this);
    //* Rows and 1 Columns

    moduleList = list;
    QStringList items;
    for (int i = 0; i < list->getSize(); i++)
    {
        items << QString(list->getModule(i)->getModuleName());
    }
    typeModel = new QStringListModel(items, this);
    ui->comBModules->setModel(typeModel);
}
//------------------------------------------------------------------------------
void LeftPanel::refreshBookList(ProjectQModuleList* list)
{
//    moduleList = list;
    myDebug() << "refresh";
    //    myDebug() << list->getSize();
    /*
     *Принимает лист модулей
     *После чего создает элементы в вью
     *Если рассматривать по алфавиту, то парсит первый модуль первую главу
     *чтобы отобразить
     *Наверное, лучше сделать отдельный виджет для показывания
     *Связать его с левой панелью и работать.
     */

//    this->modelBooks = new QStandardItemModel(moduleList->getSize(), 1, this);
//    //* Rows and 1 Columns

//    for (int i = 0; i < moduleList->getSize(); i++)
//    {
//        modelBooks->setItem(i, 0, new QStandardItem(
//                                  QString(moduleList->getModule(i)->getModuleName())));
////    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Column1 Header")));
////    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Column2 Header")));
////    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Column3 Header")));
//    }

//    ui->tableBook->setModel(modelBooks);
//    ui->tableBook->resizeColumnsToContents();

//    // Demonstrating look and feel features


//    ui->tableBook->setAnimated(false);
//    ui->tableBook->setIndentation(20);
//    ui->tableBook->setSortingEnabled(true);


}
//------------------------------------------------------------------------------
void LeftPanel::refreshChapterList(QModelIndex moind)
{
//    myDebug() << moind.data(0).toString();
    int chapterValue = moduleList->getModuleWithName(moind.data(0).toString())->getBookValue();
    modelChapters->clear();
    for (int i = 0; i < chapterValue; i++)
    {
        modelChapters->setItem(i, 0, new QStandardItem(QString::number(i + 1)));
    }
    ui->tableChapter->setModel(modelChapters);
    ui->tableChapter->resizeColumnsToContents();
//    myDebug() << chapterValue;
}
//------------------------------------------------------------------------------
void LeftPanel::init()
{
    createConnects();
    modelModules = new QStandardItemModel(0, 0, this);
    modelBooks = new QStandardItemModel(0, 0, this);
    modelChapters = new QStandardItemModel(0, 0, this);
    moduleList = new ProjectQModuleList();
}
//------------------------------------------------------------------------------
void LeftPanel::createConnects()
{
    connect(ui->comBModules, SIGNAL(activated(QString)), SLOT(refreshBookList(QString)));
    connect(ui->tableBook, SIGNAL(clicked(QModelIndex)), SLOT(refreshChapterList(QModelIndex)));
    connect(ui->tableChapter, SIGNAL(clicked(QModelIndex)), SLOT(showChapter(QModelIndex)));
}
//------------------------------------------------------------------------------
void LeftPanel::showChapter(QModelIndex ind)
{
//    myDebug() << moind.data(0).toString();
//    myDebug() << ind.data(0).toString();
//    ui->tableBook->itemDelegate

    int t_curBook;
    QModelIndexList selectedList = ui->tableBook->selectionModel()->selectedRows();
    for( int i = 0; i < selectedList.count(); i++)
    {
        t_curBook = selectedList.at(i).row();
    }


//    QString t_pathToIniFile = QString(Config::configuration()->getAppDir() + "bible/" +
//                                      info.shortName() + "/module.ini");

//    myDebug() << moduleList->getModule(t_curBook).getModulePath();
//    ModuleViewer.viewer()->showChapter(Config::configuration()->getAppDir() +
//                                       moduleList->getModule(t_curBook).getModulePath(),

//                                       );


//        myDebug() << QString::number(selectedList.at(i).row());
//            QMessageBox::information(this,"", QString::number(selectedList.at(i).row()));

//    int chapterValue = moduleList->getModuleWithName(moind.data(0).toString()).getChapterValue();
//    modelChapters->clear();
//    for (int i = 0; i < chapterValue; i++)
//    {
//        modelChapters->setItem(i, 0, new QStandardItem(QString::number(i + 1)));
//    }
//    ui->tableChapter->setModel(modelChapters);
//    ui->tableChapter->resizeColumnsToContents();
//    myDebug() << chapterValue;
}
//------------------------------------------------------------------------------
void LeftPanel::refreshBookList(QString nameOfBook)
{
    QStringList bookList = moduleList->getModuleBooks(nameOfBook);
//    myDebug() << moduleList->getModuleWithName(ui->comBModules->currentText());
//    myDebug() << bookList.size();
    for (int i = 0; i < bookList.size() - 1; i++)
    {
        modelBooks->setItem(i, 0, new QStandardItem(bookList.at(i)));
//        myDebug() << "yes";
    }

    ui->tableBook->setModel(modelBooks);
    ui->tableBook->resizeColumnsToContents();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
