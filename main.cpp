#include <QCoreApplication>
#include <QTextStream>
#include <QBitArray>
#include <QVariant>
#include <QByteArray>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    char *text = "AB";
    QBitArray b_test = QBitArray(5);
    b_test[0] = 1;
    b_test[1] = 1;
    QVariant v(b_test);
    QBitArray b_array = v.toBitArray();
    b_array ^= QBitArray(b_array.size(), true);
    b_array ^= QBitArray(b_array.size(), true);
    QString convert_text = QVariant(b_array).toString();
    return a.exec();
}
