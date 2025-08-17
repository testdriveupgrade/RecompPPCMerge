#include "idbutton.h"

#include <QTextEdit>

IdButton::IdButton(const QString &textToApply, const QString &name2, QTextEdit &target)
    : m_textToApply(textToApply), m_target(&target) {
    connect(this, &QPushButton::clicked, this, &IdButton::handleClicked);
    this->setText(name2);
}

void IdButton::handleClicked() {
    if (!m_target)
        return;
    m_target->setText(m_textToApply);
}


