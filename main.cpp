#include <QCoreApplication>
#include <QTextStream>
#include <QBitArray>
#include <QByteArray>
#include <QTime>

QBitArray ConvertByteArrayToBitArray(QByteArray byteArray);
QByteArray ConvertBitArrayToByteArray(QBitArray bitArray);
QBitArray Encryption(QBitArray bitsText, QList<QBitArray> &keyList);

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    qsrand(QTime::currentTime().second());
    QString text = "Test";
    QList<QBitArray> keys;
    QByteArray byteText = text.toLocal8Bit();
    QBitArray bitText = ConvertByteArrayToBitArray(byteText);

    QByteArray convertbitText = ConvertBitArrayToByteArray(bitText);
    QString convertText = QString::fromLocal8Bit(convertbitText);

    QBitArray bitCipher = Encryption(bitText, keys);
    QByteArray byteCipher = ConvertBitArrayToByteArray(bitCipher);
    QString ciphet = QString::fromLocal8Bit(byteCipher);

    return a.exec();
}

/**
 * @brief ConvertByteArrayToBitArray - Convert from QByteArray to BitArray
 * @param byteArray
 * @return
 */
QBitArray ConvertByteArrayToBitArray(QByteArray byteArray) {
    QBitArray bitArray(byteArray.count() * 8);

    // convert from QByteArray to BitArray
    for(int i = 0; i < byteArray.count(); ++i) {
        for (int b = 0; b < 8; b++) {
            bitArray.setBit( i*8+b, byteArray.at(i)&(1<<(7-b)) );
        }
    }

    return bitArray;
}

/**
 * @brief ConvertBitArrayToByteArray - convert from QBitArray to QByteArray
 * @param bitArray
 * @return
 */
QByteArray ConvertBitArrayToByteArray(QBitArray bitArray) {
    QByteArray byteArray;
    byteArray.resize(bitArray.size()/8);

    // convert from QBitArray to QByteArray
    for (int b = 0; b < bitArray.count(); ++b) {
        byteArray[b/8] = ( byteArray.at(b/8) | ((bitArray[b]?1:0)<<(7-(b%8))) );
    }

    return byteArray;
}

/**
 * @brief Encryption - Implements block encryption
 * @param text - source
 * @param keyList - keys for decryption
 * @return - ciphertext
 */
QBitArray Encryption(QBitArray bitsText, QList<QBitArray> &keyList) {
   int sizeText = bitsText.size();

   // add missing bits
   if (sizeText % 64 != 0) {
       int newSize = sizeText + (64 - sizeText % 64);
       QBitArray buffer(newSize);
       // copy bits
       for (int i = 0; i < sizeText; i++) {
           buffer[i] = bitsText[i];
       }
       // add missing bits
       for (int i = sizeText; i < newSize - 1; i++) {
           buffer[i] = 0;
       }
       buffer[newSize - 1] = 1;
       bitsText = buffer;
       sizeText = newSize;
   }

   // generate 64-bits key
   QBitArray bitKey = QBitArray(64);
   int key;
   for (int i = 0; i < 64; i+=16) {
       // generate 16 bit half key
       key = qrand() % 99;
       QBitArray bitBuffer = ConvertByteArrayToBitArray(QByteArray::number(key));
       // copy to key
       for (int j = 0; j < 16; j++) {
           bitKey[i + j] = bitBuffer[j];
       }
   }

   // encryption
   QBitArray encryptionText(sizeText);
   QBitArray block(64);
   for (int i = 0; i < sizeText; i+=64) {
       // add key in list
       keyList.append(bitKey);

       // generate block
       for (int j = 0; j < block.size(); j++) {
           block[i] = bitsText[i + j];
       }

       // encryption block
       QBitArray encrBlock = block ^ bitKey;
       bitKey = encrBlock;
       // add block in encrypting text
       for (int j = 0; j < block.size(); j++) {
           encryptionText[i + j] = block[j];
       }
   }
   return encryptionText;
}
