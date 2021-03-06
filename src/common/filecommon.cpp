#include "filecommon.h"
#include "stringcommon.h"
#include "strongcommon.h"

#include "debughelper.h"
#include "defines.h"

#include <QMessageBox>

//------------------------------------------------------------------------------
QString getEncodingFromFile(QString file, QString language)
{
    QProcess pr;

#ifdef Q_OS_WIN
    pr.start("enca.exe -L " +language + " \""  + file + "\"");
#endif
#ifdef Q_OS_LINUX
    pr.start("enca -L " + language + " \""  + file + "\"");
#endif
    //    pr.start("pwd");

    QString encoding = "";
    if (pr.waitForFinished() == true)
    {
        QByteArray b = pr.readAll();
        encoding = QString(b);
    }
    if (encoding.indexOf("Universal transformation format 8 bits;") >= 0 ) encoding = "UTF-8";
    if (encoding.indexOf("Universal transformation format 16 bits;") >= 0 ) encoding = "UTF-16";
    if (encoding.indexOf("Universal transformation format 32 bits;") >= 0 ) encoding = "UTF-32";
    if (encoding.indexOf("Universal character set 2 bytes; UCS-2; BMP") >= 0) encoding = "UCS-2";
    if (encoding.indexOf("MS-Windows code page 1251") >= 0) encoding = "Windows-1251";
    if (encoding.indexOf("MS-Windows code page 1252") >= 0) encoding = "Windows-1252";
    if (encoding.indexOf("MS-Windows code page 1253") >= 0) encoding = "Windows-1253";
    if (encoding.indexOf("MS-Windows code page 1254") >= 0) encoding = "Windows-1254";
    if (encoding.indexOf("MS-Windows code page 1255") >= 0) encoding = "Windows-1255";
    if (encoding.indexOf("MS-Windows code page 1256") >= 0) encoding = "Windows-1256";
    if (encoding.indexOf("MS-Windows code page 1257") >= 0) encoding = "Windows-1257";
    if (encoding.indexOf("MS-Windows code page 1258") >= 0) encoding = "Windows-1258";
    if (encoding.indexOf("7bit ASCII characters") >= 0) encoding = "ASCII";
    if (encoding.indexOf("KOI8-R Cyrillic") >= 0) encoding = "KOI8-R";
    if (encoding.indexOf("KOI8-U Cyrillic") >= 0) encoding = "KOI8-U";
    if (encoding.indexOf("Unrecognized encoding") >= 0) encoding = "UTF-8";
    //    qDebug() << "encoding = " << encoding;
    return encoding;
}

//------------------------------------------------------------------------------
QTextCodec * getCodecOfEncoding(QString encoding)
{
    //    encoding = encoding.toUpper ();
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    if (encoding.toUpper ()== "UTF-8")        codec = QTextCodec::codecForName("UTF-8");
    if (encoding.toUpper ()== "UTF-16")       codec = QTextCodec::codecForName("UTF-16");
    if (encoding.toUpper ()== "UTF-16BE")       codec = QTextCodec::codecForName("UTF-16BE");
    if (encoding.toUpper ()== "UTF-16LE")       codec = QTextCodec::codecForName("UTF-16LE");
    if (encoding.toUpper ()== "UTF-32")       codec = QTextCodec::codecForName("UTF-32");
    if (encoding.toUpper ()== "UTF-32BE")       codec = QTextCodec::codecForName("UTF-32BE");
    if (encoding.toUpper ()== "UTF-32LE")       codec = QTextCodec::codecForName("UTF-32LE");
    if (encoding == "Windows-1251") codec = QTextCodec::codecForName("Windows-1251");
    if (encoding == "Windows-1252") codec = QTextCodec::codecForName("Windows-1252");
    if (encoding == "Windows-1253") codec = QTextCodec::codecForName("Windows-1253");
    if (encoding == "Windows-1254") codec = QTextCodec::codecForName("Windows-1254");
    if (encoding == "Windows-1255") codec = QTextCodec::codecForName("Windows-1255");
    if (encoding == "Windows-1256") codec = QTextCodec::codecForName("Windows-1256");
    if (encoding == "Windows-1257") codec = QTextCodec::codecForName("Windows-1257");
    if (encoding == "Windows-1258") codec = QTextCodec::codecForName("Windows-1258");
    if (encoding.toUpper ()== "KOI8-R")       codec = QTextCodec::codecForName("KOI8-R");
    if (encoding.toUpper ()== "KOI8-U")       codec = QTextCodec::codecForName("KOI8-U");

    //        qDebug() << " encoding = " << encoding;
    return codec;
}
//------------------------------------------------------------------------------
QStringList getListWord(QString filename)
{
    QStringList r_list;
    QXmlStreamReader xmlReader;

    xmlReader.addData(getTextFromHtmlFile(filename));
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isStartElement())
        {
            QStringList sl;
            sl << xmlReader.name().toString();
            QXmlStreamAttributes attrs = xmlReader.attributes();
            //         myDebug() << attrs.value("name").toString();
            r_list.append(attrs.value("name").toString());
        }
        xmlReader.readNext();
    }
    r_list = removeEmptyQStringFromQStringList(&r_list);
    return r_list;
}
//------------------------------------------------------------------------------
void getListWordFromDict(const QString f_path, QMap<QString, QString>* f_map)
{
    f_map->clear();

    QFile* xmlFile = new QFile(f_path);
    if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QXmlStreamReader* xmlReader = new QXmlStreamReader(xmlFile);


    //Parse the XML until we reach end of it
    while(!xmlReader->atEnd() && !xmlReader->hasError())
    {
        // Read next element
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        //If token is just StartDocument - go to next
        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }
        //If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement)
        {

            if(xmlReader->name() == "word")
            {
                QXmlStreamAttributes attrs = xmlReader->attributes();

                QStringList t_list;
                QString t_text;
                t_list << xmlReader->readElementText().split("\n");
                removeEmptyQStringFromQStringList(&t_list);

                for (int i = 0; i < t_list.size(); i++)
                {
                    t_text.append(QString(t_list.at(i)).replace("    ", "") + "\n");
                }

                f_map->insert(attrs.value("name").toString(), t_text);
            }
        }
    }

    if(xmlReader->hasError())
    {
        return;
    }

    //close reader and flush file
    xmlReader->clear();
    xmlFile->close();
}
//------------------------------------------------------------------------------
bool createEmptyHtml(QString fileName, QString title, QString text)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.exists())
    {
        //create file if it's not exist
        if (file.open(QIODevice::ReadWrite))
        {
            //try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            ts << "<html>\n<head>" << endl;
            ts << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
            ts << "<title>" << title <<"</title>" << endl;
            ts << "</head>\n<body>\n" << text << "\n</body>\n</html>" << endl;
            file.close();
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
bool createEmpty(QString fileName, QString text)
{
    bool ret = false;
    QFile file(fileName);
    if (!file.exists())
    {
        //create file if it's not exist
        if (file.open(QIODevice::ReadWrite))
        {
            //try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            ts << text;
            file.close();
            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
bool createEmptyHtml(QString fileName, QString title)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.exists())
    {
        //create file if it's not exist
        if (file.open(QIODevice::ReadWrite))
        {
            //try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            ts << "<html>\n<head>" << endl;
            ts << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
            ts << "<title>" << title <<"</title>" << endl;
            ts << "</head>\n<body>\n</body></html>" << endl;
            file.close();
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
void replaceStrInFile(QString filepath, QString old, QString newstr)
{
    QFile file(filepath);
    QString str;
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        stream.setCodec(getCodecOfEncoding("UTF-8"));
        str = stream.readAll();
        str.replace(old, newstr);
    }
    else
    {
        qDebug() << "Error: not open file (replacestrinfile):" << filepath;
    }
    file.close();

    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream.setCodec(getCodecOfEncoding("UTF-8"));
        stream << str;
        file.close();
    }
    else
    {
        qDebug() << "Error: not open file (replacestrinfile write):" << filepath;
    }

}
//------------------------------------------------------------------------------
QString getTextFromHtmlFile(QString filePath)
{
    QString str = "";
    QFile file(filePath);
    //    qDebug() << filePath;
    file.close();
    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            stream.setCodec(getCodecOfEncoding(getEncodingFromFile(filePath)));
            str = stream.readAll();
            file.close();
        }
        else
        {
            myDebug() << "Error: not open file(getTextFromHtmlFile):" << filePath;
        }
    }
    return str;
}
//------------------------------------------------------------------------------
QStringList getInfoFromFile(QString file_path)
{
    QStringList list;
    if(QFile::exists(file_path))
    {
        QString encoding = getEncodingFromFile(file_path);
        QFile file(file_path);
        QString str;
        QString info[7];
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            stream.setCodec(getCodecOfEncoding(encoding));
            do
            {
                str = stream.readLine();
                if (str.indexOf("<meta name=\"Author\"") != -1)
                    info[0] = getParamInfo(&str, "Author");
                if (str.indexOf("<meta name=\"Revision\"") != -1)
                    info[1] = getParamInfo(&str, "Revision");
                if (str.indexOf("<meta name=\"Language\"") != -1)
                    info[2] = getParamInfo(&str, "Language");
                if (str.indexOf("<meta name=\"Type\"") != -1)
                    info[3] = getParamInfo(&str, "Type");
                if (str.indexOf("<meta name=\"Description\"") != -1)
                    info[4] = getParamInfo(&str, "Description");
                if (str.indexOf("<meta name=\"Rights\"") != -1)
                    info[5] = getParamInfo(&str, "Rights");
                if (str.indexOf("<meta name=\"Numbering\"") != -1)
                    info[6] = getParamInfo(&str, "Numbering");
            } while(!stream.atEnd());
            file.close();

            for (int i = 0; i < 7; i++)
            {
                list << info[i];
            }
        }
        else
        {
            qDebug() << "Error: not open file for read(get info from file):"
                     << file_path;
        }
    }

    return list;
}
//------------------------------------------------------------------------------
QString getParamInfo(QString *inputstr, QString param)
{
    QString str = *inputstr;
    QString remove = "<meta name=\"" + param + "\" content=\"";
    str.remove(remove)
            .remove("\">");
    str = removeSpaces(str);

    if (str == "RU")
        str = "rus";

    if (str == "En")
        str = "eng";

    return str;
}
//-------------------------------------------------------------------------------
QString findPosWord(QString file, QString text)
{
    QString t_output_str = "";
    if(QFile::exists(file))
    {
        QString t_text = getTextFromHtmlFile(file);
        QStringList t_list = t_text.split("\n");

        QString t_find_text = text;
        bool flag = true;
        do
        {
            for (int i = 0; i < t_list.size(); i++)
            {
                if (QString(t_list.at(i)).indexOf(t_find_text) != -1)
                {
                    t_output_str = t_list.at(i);
                    flag = false;
                    break;
                }
            }
            t_find_text.remove(t_find_text.length() -1, 1);
        } while(flag and
                !t_find_text.isEmpty());
    }
    return t_output_str;
}
//------------------------------------------------------------------------------
bool createEmptyXML(QString fileName)
{
    bool ret = false;
    QFile file(fileName);
    if (!file.exists())
    {
        //create file if it's not exist
        if (file.open(QIODevice::ReadWrite))
        {
            //try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            //            ts << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
            //            ts << "<!DOCTYPE xbel>" << endl;
            //            ts << "<xbel version=\"1.0\">" << endl;
            ts << "<xml>" << endl;
            //            ts << "</xml>" << endl;
            file.close();
            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
bool addBookToXML(QString fileName, QString namebook, Book mbook)
{
    bool ret = false;
    //    myDebug() << getTextFromHtmlFile(fileName);
    //    myDebug() << "\n\n\n\n";
    QFile file(fileName);
    if (file.exists())
    {
        //create file if it's not exist
        if (file.open(QIODevice::Append))
        {
            //try to open or create file
            QString tab = "    ";
            QTextStream ts(&file);
            ts.setCodec(getCodecOfEncoding(getEncodingFromFile(fileName)));
            ts << tab << "<book name=\"" << namebook << "\">" << endl;
            for (int i = 0; i < mbook.size(); i++)
            {

                ts << tab << tab << "<chapter number=\"" << i + 1
                   << "\">" << endl;
                for (int j = 0; j < mbook.getChapter(i).verseCount(); j++)
                {
                    QString text = mbook.getChapter(i).data().value(j).data();

                    QString str = getClearText(&text);
                    ts << tab << tab << tab << str;
                }

                ts  << tab << tab << "</chapter>" << endl;
            }

            ts << tab << "</book>" << endl;
            file.close();
            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
bool endXML(QString fileName)
{
    bool ret = false;
    QFile file(fileName);
    if (file.exists())
    {
        //create file if it's not exist
        if (file.open(QIODevice::Append))
        {
            //try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            //            ts << "</xbel>" << endl;
            ts << "</xml>" << endl;
            file.close();
            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
QSopherimModuleInfo getModuleInfo(QString fileName)
{

    QSopherimModuleInfo list;
    list.moduleName = getParamModule(fileName, "ModuleName");
    list.moduleShortName = getParamModule(fileName, "ModuleShortName");
    //    list.append(getParamModule(fileName, "ModuleLanguage"));
    list.bookValue = getParamModule(fileName, "BooksValue").toInt();
    list.modulePath = getParamModule(fileName, "PathToModule");
    list.moduleType = getParamModule(fileName, "TypeModule");

    list.bookList = getBookList(fileName);

    list.numberOfChaptersInBook = getNumberOfChaptersInBook(fileName);
    //    myDebug() << list.numberOfChaptersInBook.size();
    //    myDebug() << list.bookList.size();
    //    list.append(getParamModule(fileName, "ModuleShortName"));

    return list;
}
//------------------------------------------------------------------------------
QString getParamModule(QString filename, QString param)
{
    // translate to hindi

    QString str = "";
    QString line;
    QString parama = param + " = ";

    QFile file(filename);

    if (file.isOpen())
        file.close();

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream( &file );
        do
        {
            line = stream.readLine();
            if (line.indexOf(parama) >= 0)
            {
                str = line.remove(parama);
            }
        } while (str.isEmpty() and !line.isNull());

        file.close();
    }
    else
    {
        myDebug() << "Error: not get param. Param: " << param;
    }
    return str;
}
//------------------------------------------------------------------------------
QStringList getBookList(QString file)
{
    QStringList bookList;
    QString param = "BookList";
    // translate to hindi
    QString str = getParamModule(file, param);
    bookList << str.split(GL_SYMBOL_SPLIT_BOOK);
    bookList = removeEmptyQStringFromQStringList(&bookList);
    return bookList;
}
//------------------------------------------------------------------------------
QHash<QString, int> getNumberOfChaptersInBook(QString filename)
{

    QStringList bookList;
    QString param = "NumberChapter";
    //     translate to hindi
    QString str = getParamModule(filename, param);
    bookList << str.split(GL_SYMBOL_SPLIT_CHAPTER);
    bookList = removeEmptyQStringFromQStringList(&bookList);
    QHash<QString, int> list;

    for (int i = 0; i < bookList.size(); i++)
    {
        QStringList test;
        test << bookList.at(i).split("^");
        //        myDebug() <<  bookList.at(i) << test.size();

        list[test.at(0)] = test.at(1).toInt();
    }
    return list;
}


//-------------------------------------------------------------------------------
QStringList getListModulesFromPath(QString path, QString format)
{
    QStringList files = recursiveFind(path);
    QStringList list;
    for(int i = 0; i < files.size(); i++)
    {
        if(files.at(i).indexOf(format) >= 0
                or files.at(i).indexOf(format.toUpper()) >= 0)
        {
            list << files.at(i);
        }
    }
    return list;
}
//-------------------------------------------------------------------------------
QStringList recursiveFind(QString directory)
{
    //// подумать, нужен ли чистый рекурсивный поиск
    //// если нет, то сюда вставить обработку getModuleFilesList
    QStringList list;
    QDirIterator iterator (directory, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while(iterator.hasNext())
    {
        iterator.next();
        list << iterator.fileInfo().absoluteFilePath();
    }
    return list;
}
//-------------------------------------------------------------------------------
//QString getVerseNumberFromNote(QString* line)
//{

//    QString str = *line;
//    QString t_str ="verse=\"";
//    int pos = str.indexOf(t_str);
//    int pos2 = str.indexOf("\"", pos + t_str.length());

//    str = str.mid(pos + t_str.length(), pos2  - pos - t_str.length());
//    return str;
//}
//-------------------------------------------------------------------------------
QString getDescriptionForWordFromDict(QString t_pathToFile, QString word)
{
    QXmlStreamReader xmlReader;
    QString r_str;
    xmlReader.addData(getTextFromHtmlFile(t_pathToFile));
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isStartElement())
        {
            QStringList sl;
            sl << xmlReader.name().toString();
            QXmlStreamAttributes attrs = xmlReader.attributes();
            if (attrs.value("name") == word)
            {
                r_str = xmlReader.readElementText();
                r_str.remove("    ");
                break;
            }
        }
        //        if (xmlReader.isEndElement())
        //        {

        //        }
        xmlReader.readNext();
    }
    return r_str;
}
//-------------------------------------------------------------------------------
QStringList getBookmarks(QString pathToFile)
{
    QXmlStreamReader xmlReader;
    //    QString r_str;
    QStringList r_list;
    xmlReader.addData(getTextFromHtmlFile(pathToFile));
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isStartElement())
        {
            QStringList sl;
            sl << xmlReader.name().toString();
            QXmlStreamAttributes attrs = xmlReader.attributes();
            r_list  << attrs.value("name").toString();
        }
        //        if (xmlReader.isEndElement())
        //        {

        //        }
        xmlReader.readNext();
    }
    r_list = removeEmptyQStringFromQStringList(&r_list);
    return r_list;
}
//-------------------------------------------------------------------------------
void addToListBookModule(const QString f_shortName)
{
    myDebug() << "apend book module " << f_shortName;
}
//-------------------------------------------------------------------------------
void addToListBibleModule(const QString f_shortName)
{
    myDebug() << "apend bible module " << f_shortName;
}
//-------------------------------------------------------------------------------
QString getModuleNameFromIni(const QString f_filePath)
{
    QString r_moduleName;
    if(QFile::exists(f_filePath))
    {
        QString encoding = getEncodingFromFile(f_filePath);
        QFile file(f_filePath);
        QString str;
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            stream.setCodec(getCodecOfEncoding(encoding));
            do
            {
                str = stream.readLine();
                if(str.contains("ModuleName", Qt::CaseInsensitive))
                {
                    r_moduleName = getParamFromStr(&str, "ModuleName");
                }

            } while(!stream.atEnd());
            file.close();
        }
        else
        {
            qDebug() << "Error: not open file for read(get info from file):"
                     << f_filePath;
        }
    }
    return r_moduleName;
}
//------------------------------------------------------------------------------
QStringList getReadinPlanForDay(const int f_mount, const int f_day, const QString f_type)
{
    QStringList r_list;

    switch (f_mount)
    {
    case MOUNT_JANUARY: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/01.xml"); break;
    case MOUNT_FEBRUARY: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/02.xml"); break;
    case MOUNT_MARCH: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/03.xml"); break;
    case MOUNT_APRIL: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/04.xml"); break;
    case MOUNT_MAY: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/05.xml"); break;
    case MOUNT_JUNE: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/06.xml"); break;
    case MOUNT_JULY: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/07.xml"); break;
    case MOUNT_AUGUST: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/08.xml"); break;
    case MOUNT_SEPTEMBEER: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/09.xml"); break;
    case MOUNT_OCTOBER: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/10.xml"); break;
    case MOUNT_NOVEMBER: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/11.xml"); break;
    case MOUNT_DECEMBER: r_list = getReadingPlanForDayFromFile(f_day, Config::configuration()->getDataPath() + GL_PLAN_PATH + f_type + "/12.xml"); break;
    default:
        break;
    }

    return r_list;
}
//------------------------------------------------------------------------------
QStringList getReadingPlanForDayFromFile(const int f_day, const QString f_path)
{
    QStringList r_list;
    QStringList t_list(getTextFromHtmlFile(f_path).split("\n"));

    for (int i = 0; i < t_list.size(); i++)
    {
        if (t_list.at(i).indexOf(QString("day=\'%1\'").arg(f_day)) >= 0)
        {
            int pos1 = t_list.at(i).indexOf(">") + 1;
            int pos2 = t_list.at(i).indexOf("<", pos1);

            r_list << t_list.at(i).mid(pos1, pos2 - pos1);
        }
    }
    return r_list;
}
//------------------------------------------------------------------------------
QString getCommentForChapter(const QString f_path, const QString f_book, const QString f_chapter)
{
    // hindi
    QString r_str = "";

    //    myDebug() << isExistBook(f_path, f_book);

    QString t_findBook;
    if (isExistBook(f_path, f_book, &t_findBook));
    {
        if (!t_findBook.isEmpty())
        {
            QString t_path = f_path;
            t_path.replace("module" + GL_FORMAT_MODULE
                           , "text" + GL_FORMAT_TEXT);

            QXmlStreamReader xmlReader;
            xmlReader.addData(getTextFromHtmlFile(t_path));

            bool flag = false;
            while(!xmlReader.atEnd() and !flag)
            {
                if(xmlReader.isStartElement())
                {
                    QStringList sl;
                    sl << xmlReader.name().toString();
                    QXmlStreamAttributes attrs = xmlReader.attributes();
                    if (attrs.value("name").toString() == t_findBook)
                    {
                        while(!xmlReader.atEnd() and !flag)
                        {
                            if (xmlReader.attributes().value("number") == f_chapter)
                            {
                                flag = true;
                                QString str = xmlReader.readElementText();
                                str.remove("    ");
                                r_str = str;
                            }
                            xmlReader.readNext();
                        }
                    }
                }
                xmlReader.readNext();
            }
        }
    }
    return r_str;
}
//------------------------------------------------------------------------------
bool isExistBook(const QString f_path, const QString f_book, QString* r_bookName)
{
    bool r_bool = false;
    QString t_str = getParamModule(f_path, "BookList");
    QStringList t_list(t_str.split(GL_SYMBOL_SPLIT_BOOK));
    int t_number = getNumberOfBook(f_book);
    int i = 0;
    do
    {
        if (!t_list.at(i).isEmpty()
                and getNumberOfBook(t_list.at(i)) == t_number)
        {
            r_bool = true;
            *r_bookName = t_list.at(i);
        }
        i++;
    } while (!r_bool and i < t_list.size());

    return r_bool;
}
//------------------------------------------------------------------------------
QMap<int, QString> getNoteOfParams(QString f_module, QString f_book, QString f_chapter, QString f_path)
{
    QMap<int, QString> r_map;

    QString t_text = getTextFromHtmlFile(f_path);
    QStringList t_list;
    int t_count = 0;
    t_text.remove("<xml>").remove("</xml>");
    t_list << t_text.split("</note>");
    t_list = removeEmptyQStringFromQStringList(&t_list);
    for (int i = 0; i < t_list.size(); i++)
    {
        QString str1 = "module=\"" + f_module + "\"";
        QString str2 = "book=\"" + f_book + "\"";
        QString str3 = "chapter=\"" + f_chapter + "\"";
        QString t_line = t_list.at(i);
        if (t_line.contains(str1) &&
                t_line.contains(str2) &&
                t_line.contains(str3))
        {
            // remove tag before text
            int pos = t_line.indexOf(">");
            t_line.remove(0, pos + 1);
            r_map[t_count] = t_line;
            t_count++;
        }
    }
    return r_map;
}
//-------------------------------------------------------------------------------
bool toLog(QString logFN, QString logMessage)
{
    QFile file(logFN);
    if(!file.open(QIODevice::Append | QIODevice::Text))
    {
        return false;
    }
    QTextStream ts(&file);
    //ts.setCodec("UTF-8"); //do not set codec, write in system codepage yet
    QDateTime dt = QDateTime::currentDateTime();
    ts << dt.toString("yyyy.MM.dd hh:mm:ss  ") << logMessage << endl;
    file.close();
    return true;
}
//-------------------------------------------------------------------------------
QString getBookNameForNumberForModule(const QString f_module, const QString f_bookName)
{
    QString r_str = f_bookName;

    QStringList t_list = Config::configuration()->getListModulesFromMap(Config::configuration()->getTypeOfModule(f_module))
            ->getModuleWithName(f_module)->getBookList();
    int t_number = getNumberOfBook(f_bookName);

    for (int i = 0; i < t_list.size(); i++)
    {
        if (t_number == getNumberOfBook(t_list.at(i)))
        {
            r_str = t_list.at(i);
            break;
        }
    }
    return r_str;
}
//-------------------------------------------------------------------------------
