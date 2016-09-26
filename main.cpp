#include <QCoreApplication>
#include <QTextStream>
#include <QBitArray>
#include <QByteArray>
#include <QTime>

QBitArray ConvertByteArrayToBitArray(QByteArray byteArray);
QByteArray ConvertBitArrayToByteArray(QBitArray bitArray);
QBitArray Encryption(QBitArray bitsText, QList<QBitArray> &keys);
QBitArray Decoding(QBitArray bitCipher, QList<QBitArray> keys);

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    qsrand(QTime::currentTime().second());
    QString text = "T";
    QList<QBitArray> keys;
    QByteArray byteText = text.toLocal8Bit();
    QBitArray bitText = ConvertByteArrayToBitArray(byteText);

    QBitArray testKey = QBitArray(bitText.size());
    for (int i = 0; i < testKey.size(); i++) {
        // generate 16 bit half key
        int key = qrand() % 100;
        if (key < 50)
            testKey[i] = true;
        else
            testKey[i] = false;
    }


    QBitArray testCipher = bitText ^ testKey;

    QBitArray testBitDecodeText = testCipher ^ testKey;

    QString decText = QString::fromLocal8Bit(
                ConvertBitArrayToByteArray(testBitDecodeText)
                );

    // Encryption
    QBitArray bitCipher = Encryption(bitText, keys);

    // remove unnecessary bits
    QBitArray buffer(bitText.size());
    for (int i = 0; i < bitText.size(); i++) {
        buffer[i] = bitCipher[i];
    }
    QString cipher = QString::fromLocal8Bit(
                ConvertBitArrayToByteArray(buffer)
                );
    // decoding
    QBitArray bitDecodeText = Decoding(bitCipher, keys);
    for (int i = 0; i < bitText.size(); i++) {
        buffer[i] = bitDecodeText[i];
    }
    QString decodeText = QString::fromLocal8Bit(
                ConvertBitArrayToByteArray(buffer)
                );

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
QBitArray Encryption(QBitArray bitsText, QList<QBitArray> &keys) {
    int sizeText = bitsText.size();
    // text add empty bits for block
    QBitArray fullBitText;
    int fullSize;
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
       fullBitText = buffer;
       fullSize = newSize;
   }

   // generate 64-bits key
   QBitArray bitKey = QBitArray(64);
   int key;
   for (int i = 0; i < 64; i++) {
       // generate 16 bit half key
       key = qrand() % 99;
       bitKey[i] = key < 50 ? true : false;
   }

   // encryption
   QBitArray encryptionText(fullSize);
   QBitArray block(64);
   for (int i = 0; i < fullSize; i+=64) {
       // add key in list
       keys.append(bitKey);

       // generate block
       for (int j = 0; j < block.size(); j++) {
           block[i] = fullBitText[i + j];
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

/**
 * @brief Decoding
 * @param bitCipher - raw data encrypting input text
 * @param keys - keys to encryption
 * @return decoding text
 */
QBitArray Decoding(QBitArray bitCipher, QList<QBitArray> keys) {
    QBitArray block(64); // block
    QBitArray decoderText(bitCipher.size());
    QBitArray key;
    QListIterator<QBitArray> keysIter(keys);
    QBitArray text(bitCipher.size());
    // decoding
    for (int i = 0; i < bitCipher.size(); i+=64) {

        // get block
        for (int j = 0; j < block.size(); j++) {
            block[j] = bitCipher[i + j];
        }
        // get key
        key = keysIter.next();

        // xor
        QBitArray decBlock = block ^ key;
        // add block in decoding text
        for (int j = 0; j < block.size(); j++) {
            text[i + j] = decBlock[j];
        }
    }
    return text;
}
