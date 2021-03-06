/**************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
**************************************************************************/

#include <updateoperations.h>
#include <utils.h>
#include <packagemanagercore.h>
#include <binarycontent.h>
#include <settings.h>
#include <fileutils.h>
#include <init.h>

#include <QDir>
#include <QObject>
#include <QTest>
#include <QFile>
#include <QDebug>

using namespace KDUpdater;
using namespace QInstaller;

class tst_moveoperation : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m_testDirectory = QInstaller::generateTemporaryFileName();
        QVERIFY(QDir().mkpath(m_testDirectory));
        QVERIFY(QDir(m_testDirectory).exists());

        QString testFile = QDir::toNativeSeparators("/testFile.txt");
        m_sourceFile = qApp->applicationDirPath() + testFile;
        QFile file(m_sourceFile);
        QVERIFY(file.open(QIODevice::WriteOnly)); //Generates the m_sourceFile
        file.close();

        m_destinationDirectory = m_testDirectory + QDir::toNativeSeparators("/test");
        QVERIFY(QDir().mkpath(m_destinationDirectory));
        m_destinationFile = m_destinationDirectory + QDir::toNativeSeparators(testFile);
    }

    void testMissingArguments()
    {
        MoveOperation op;

        QVERIFY(op.testOperation());
        QVERIFY(!op.performOperation());

        QCOMPARE(UpdateOperation::Error(op.error()), UpdateOperation::InvalidArguments);
        QCOMPARE(op.errorString(), QString("Invalid arguments in Move: "
                                           "0 arguments given, exactly 2 arguments expected."));
    }

    void testMoveFile()
    {
        MoveOperation op;
        op.setArguments(QStringList() << m_sourceFile << m_destinationFile);
        op.backup();
        QVERIFY2(op.performOperation(), op.errorString().toLatin1());
        QVERIFY(!QFile::exists(m_sourceFile));
        QVERIFY(QFile::exists(m_destinationFile));

        QVERIFY(op.undoOperation());
        QVERIFY(QFile::exists(m_sourceFile));
        QVERIFY(!QFile::exists(m_destinationFile));
    }

    void testMoveFileDestinationExists()
    {
        QFile file(m_destinationFile);
        QVERIFY(file.open(QIODevice::WriteOnly)); //Creates the destination file
        file.close();

        MoveOperation op;
        op.setArguments(QStringList() << m_sourceFile << m_destinationFile);
        op.backup();
        QVERIFY2(op.performOperation(), op.errorString().toLatin1());
        QVERIFY(!QFile::exists(m_sourceFile));
        QVERIFY(QFile::exists(m_destinationFile));

        QVERIFY(op.undoOperation());
        QVERIFY(QFile::exists(m_sourceFile));
        QVERIFY(QFile::exists(m_destinationFile));
    }

    void testPerformingFromCLI()
    {
        QInstaller::init(); //This will eat debug output
        PackageManagerCore *core = new PackageManagerCore(BinaryContent::MagicInstallerMarker, QList<OperationBlob> ());
        QString appFilePath = QCoreApplication::applicationFilePath();
        core->setAllowedRunningProcesses(QStringList() << appFilePath);
        QSet<Repository> repoList;
        Repository repo = Repository::fromUserInput(":///data/repository");
        repoList.insert(repo);
        core->settings().setDefaultRepositories(repoList);

        core->setValue(scTargetDir, m_testDirectory);
        core->installDefaultComponentsSilently();

        QFile movedFile(m_testDirectory + QDir::separator() + "DestinationFolder/testFile.txt");
        QVERIFY(movedFile.exists());
        QFile originalFile(m_sourceFile);
        QVERIFY(!originalFile.exists());

        core->setPackageManager();
        core->commitSessionOperations();

        core->uninstallComponentsSilently(QStringList() << "A");

        QVERIFY(!movedFile.exists());
        QVERIFY(originalFile.exists());

        core->deleteLater();
    }

    void cleanupTestCase()
    {
        QDir dir(m_testDirectory);
        QVERIFY(dir.removeRecursively());
        QVERIFY(QFile::remove(m_sourceFile));
    }

private:
    QString m_testDirectory;
    QString m_sourceFile;
    QString m_destinationDirectory;
    QString m_destinationFile;
};

QTEST_MAIN(tst_moveoperation)

#include "tst_moveoperation.moc"
