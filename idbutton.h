#pragma once

#include <QPushButton>
#include <QPointer>
#include <QTextEdit>
#include <QString>

class IdButton : public QPushButton {
    Q_OBJECT
public:
    explicit IdButton(const QString &textToApply,const QString &name2, QTextEdit &target);

    QString textToApply() const { return m_textToApply; }
    void setTextToApply(const QString &text) { m_textToApply = text; }

   QTextEdit *target() const { return m_target.data(); }
    void setTarget(QTextEdit *target) { m_target = target; }

private slots:
    void handleClicked();

private:
    QString m_textToApply;
    QPointer<QTextEdit> m_target;
};


