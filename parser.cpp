#include "parser.h"

Parser::Parser(const QString& filename):
filename_(filename)
{
    // Populating the list...
    readStreams();
}

void Parser::readStreams()
{
/*
This function opens the file as read-only and reads it line by line, looking
for all valid URL entries. It extracts and inserts each of them, with their
corresponding descriptions, into a memory structure (a list).

Example entry:
    stream_data[1]: "http://pub1.sky.fm/sky_cafedeparis|SKY.FM - Café de Paris"
*/
  // Opening file:
  QFile file(filename_);
  if (!file.open(QIODevice::ReadOnly|QIODevice::Text)){
      return;
  }

  QTextStream in(&file);
  in.setCodec("UTF-8");

  // Scanning and adding entries to list:
  while (!in.atEnd()) {
    QString l = in.readLine();

    // Save the definition line (just in case it can't be an arbitrary name).
    if (l.contains("live_stream_def", Qt::CaseInsensitive)) {
        live_stream_def_line_ = l;
        continue;
    }

    int first_quote 			= l.indexOf('"');
    if (first_quote != -1) {    // Is it a stream_data[] definition?
        int last_quote          = l.lastIndexOf('"');
        int separator           = l.indexOf('|');

        QString url             = l.mid(first_quote+1, separator-first_quote-1);
        QString name            = unescapeString(l.mid(separator+1, last_quote-separator-1));

        Stream s(url, name);
        streams_.push_back(s);
    }
  }
    file.close();
}

QString Parser::unescapeString(const QString& str) const {
/* This function looks for and unescapes unicode characters encoded like string literals.
For example: "Rádio" appears as "R\xc3\xa1dio" in the .sii file.
*/
    QString res;
    int i = 0;
    while (i < str.length()) {
        if (str[i] == '\\') { // Found an escape character.
            // Reading the character's 2 bytes after "\x".
            // In the string they are represented by 8 characters ("\xh1h0\xl1l0").
            QString high    = str.mid(i+2, 2);  // High byte: "c3"
            QString low     = str.mid(i+6, 2);  // Low byte: "a1"
            i = i + 8;

            QString code_str(high);             // Raw bytes as a string.
            code_str.append(low);

            // Decoding the bytes and getting the codepoint.
            // For example: codepoint('á') == 0xE1.
            QChar c(getCodePoint(code_str.toInt(NULL, 16)));
            res.append(c);
        }
        else {
            res.append(str[i]);
            i++;
        }
    }

    return res;
}

QString Parser::escapeString(const QString& str) const {
    QString res;

    for (int i = 0; i < str.length(); i++) {
        if (str[i].unicode() > 127) { // Found a unicode character.
            QByteArray ca;
            ca.append(str[i]);
            ca = ca.toHex();        // Now ca has the code in hexa. Ex: 0xC3A1
            bool ok = false;
            unsigned int code = ca.toUInt(&ok, 16);
            if (!ok) {
                continue;
            }
            unsigned int high = (code & 0xFF00)>>8; // High = 0xC3.
            unsigned int low = code & 0x00FF;       // Low = 0xA1.
            QString high_str = QString::number(high, 16);   // Converting both parts into strings.
            QString low_str = QString::number(low, 16);     // Format: "\xH1H0\xL1L0".
            QString escaped = "\\x" + high_str + "\\x" + low_str;

            res.append(escaped);
        }
        else { // Regular, ASCII character.
            res.append(str[i]);
        }
    }

   return res;
}

QChar Parser::getCodePoint(unsigned int word) const {
/*
Example: 0xC3A1 == (110)0 0000 (10)00 0000
According to the UTF-8 standard, bits outside of () represent the character.
The three most significant bits determine the length in bytes that need to be read.
*/
    unsigned int high   = (word & 0x1F00);
    unsigned int low    = (word & 0x003F);

    return (high>>2) | low ;
}

StreamList::iterator Parser::streamsBegin()
{
    StreamList::iterator res = streams_.begin();
    return res;
}

StreamList::const_iterator Parser::streamsBegin() const
{
    StreamList::const_iterator res = streams_.begin();
    return res;
}

StreamList::iterator Parser::streamsEnd()
{
    StreamList::iterator res = streams_.end();
    return res;
}

StreamList::const_iterator Parser::streamsEnd() const
{
    StreamList::const_iterator res = streams_.end();
    return res;
}


void Parser::swapStreams(unsigned int a, unsigned int b)
{
    /*StreamList::iterator it1(this->streamsBegin());
    StreamList::iterator it2(this->streamsBegin());
    advance(it1, a);
    advance(it2, b);
    //swap(*it1, *it2);*/
    Stream temp = streams_[a];
    streams_[a] = streams_[b];
    streams_[b] = temp;
}

void Parser::deleteStream(unsigned int s)
{
    /*StreamList::iterator it(this->streamsBegin());
    advance(it, s);
    streams_.erase(it);*/
    streams_.removeAt(s);
}

bool Parser::saveStreams()
{
    return(this->saveStreams(filename_));
}

bool Parser::saveStreams(const QString& filename)
{
/*
This function saves the current information contained in the streams_ list in
the .sii format. It overwrites the previous contents.
*/
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out.setCodec("ASCII");

    // Header
    out << "SiiNunit" << endl;
    out << "{" << endl;
        out << live_stream_def_line_ << endl;
        out << " stream_data: " << streams_.size() << endl;
    // /Header

    // Items (stream_data[n]: "http://.com|Name")
    unsigned int i = 0;
    StreamList::const_iterator it = this->streams_.begin();
    for (; it != streams_.end(); it++) {
        out << " stream_data[" << i++ << "]: ";
        out << '\"' << it->url << "|" << escapeString(it->name) << '\"';
        out << endl;
    }
    // /Items

    // Close
        out << "}" << endl;
    out << "}" << endl;

    file.close();
    return true;
}

void Parser::insertStream(const Stream& s)
{
    this->streams_.push_back(s);
}
