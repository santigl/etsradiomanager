#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QDir>
#include <QTextStream>

struct Stream {
    QString url;
    QString name;

    Stream(QString u, QString d): url(u), name(d) {};
};

//typedef std::list<Stream> StreamList;
typedef QList<Stream> StreamList;



class Parser {
public:
    Parser(const QString&);
    bool saveStreams();                 // Overwrite input file.
    bool saveStreams(const QString&);   // Save to new file.
    StreamList::iterator streamsBegin();
    StreamList::const_iterator streamsBegin() const;
    StreamList::iterator streamsEnd();
    StreamList::const_iterator streamsEnd() const;

    void swapStreams(unsigned int, unsigned int);
    void deleteStream(unsigned int);
    void insertStream(const Stream&);

private:
    StreamList streams_;
    QString filename_;
    QString live_stream_def_line_;

    void readStreams();
    QString unescapeString(const QString&) const;
    QString escapeString(const QString&) const;
    QChar getCodePoint(unsigned int byte) const;
};

#endif // PARSER_H
