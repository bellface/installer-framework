/**************************************************************************
**
** Copyright (C) 2012-2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
**
** $QT_END_LICENSE$
**
**************************************************************************/

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "installer_global.h"

#include <QJSValue>
#include <QJSEngine>

namespace QInstaller {

class PackageManagerCore;

class INSTALLER_EXPORT ScriptEngine : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ScriptEngine)

public:
    explicit ScriptEngine(PackageManagerCore *core = 0);

    QJSValue globalObject() const { return m_engine.globalObject(); }
    QJSValue newQObject(QObject *object);
    QJSValue evaluate(const QString &program, const QString &fileName = QString(),
        int lineNumber = 1);

    void addQObjectChildren(QObject *root);
    void removeQObjectChildren(QObject *root);

    QJSValue loadInContext(const QString &context, const QString &fileName,
        const QString &scriptInjection = QString());
    QJSValue callScriptMethod(const QJSValue &context, const QString &methodName,
        const QJSValueList &arguments = QJSValueList());

private slots:
    void setGuiQObject(QObject *guiQObject);

private:
    QJSValue generateMessageBoxObject();
    QJSValue generateQInstallerObject();
    QJSValue generateWizardButtonsObject();
    QJSValue generateDesktopServicesObject();

private:
    QJSEngine m_engine;
    QHash<QString, QStringList> m_callstack;
};

}
Q_DECLARE_METATYPE(QInstaller::ScriptEngine*)

#endif // SCRIPTENGINE_H
