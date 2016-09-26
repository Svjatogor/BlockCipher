#include "mainwindow.h"
#include "ui_mainwindow.h"

QByteArray Encryption(QByteArray bytesText, QList<QByteArray> &keys);
QByteArray Decoding(QByteArray bytesCipher, QList<QByteArray> keys);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Encryption - Implements blocks encryption
 * @param bytesText - input text
 * @param keyList - keys for decryption
 * @return - ciphertext
 */
QByteArray Encryption(QByteArray bytesText, QList<QByteArray> &keys) {

    int sizeText = bytesText.size();
    // add missing bits
    if (sizeText % 8 != 0) {
        int newSize = sizeText + (8 - sizeText % 8);
        for (int i = 0; i < newSize - sizeText; i++) {
            bytesText.append('z');
        }
    }
    sizeText = bytesText.size();

    // generate 64-bits key
    int keySize = 64;
    QByteArray key;
    int k;
    for (int i = 0; i < keySize / 8; i++) {
        // generate 1 byte for key
        k = qrand() % 255;
        key[i] = k;
    }

    // encryption
    QByteArray encryptionText;
    QByteArray encrBlock;
    QByteArray block;
    for (int i = 0; i < sizeText;) {
        // add key in list
        keys.append(key);

        // generate block
        block = bytesText.mid(i, keySize / 8);

        encrBlock = QByteArray();
        // encryption block
        for (int j = 0; j < block.size(); j++) {
            encrBlock.append(block[j] ^ key[j]);
        }
        // next key it is block
        key = encrBlock;

        // add block in encrypting text
        foreach (char byte, encrBlock) {
            encryptionText.append(byte);
        }

        // next block
        i += 8;
    }

    return encryptionText;
}

/**
 * @brief Decoding
 * @param bitCipher - raw data encrypting input text
 * @param keys - keys to encryption
 * @return decoding text
 */
QByteArray Decoding(QByteArray bytesCipher, QList<QByteArray> keys) {
    int blockSize = 64;
    QByteArray decoderText;
    QByteArray key;
    QListIterator<QByteArray> keysIter(keys);
    QByteArray block;
    // decoding
    for (int i = 0; i < bytesCipher.size();) {

        // get key
        key = keysIter.next();

        // get block
        block = bytesCipher.mid(i, key.size());

        // xor
        QByteArray decoderBlock;
        for (int j = 0; j < block.size(); j++) {
            decoderBlock[j] = block[j] ^ key[j];
        }

        // add block in decoding text
        foreach (char byte, decoderBlock) {
            decoderText.append(byte);
        }

        // next block
        i += 8;
    }
    return decoderText;
}

void MainWindow::on_encryptButton_clicked()
{
    QString inputText = ui->inputText->toPlainText();
    QList<QByteArray> keys;
    QByteArray bytesEncryptingText = Encryption(inputText.toLocal8Bit(), keys);
    ui->encryptText->setPlainText(QString::fromLocal8Bit(bytesEncryptingText));
}
