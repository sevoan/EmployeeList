#ifndef FORMVALIDATORRULES_H
#define FORMVALIDATORRULES_H

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

#include "formvalidator.h"

namespace FormValidatorRules {

    class __ComboBoxNotEmptyRule : public ValidityRule {
    public:
        virtual bool isValid(const QWidget* widget) const {
            return qobject_cast<const QComboBox*>(widget)->currentIndex() != -1;
        }
    };

    class __LineEditNotEmptyRule : public ValidityRule {
    public:
        virtual bool isValid(const QWidget* widget) const {
            return !qobject_cast<const QLineEdit*>(widget)->text().isEmpty();
        }
    };

    static ValidityRules _comboBoxRules;
    static ValidityRules _lineEditRules;

    static ValidityRules comboBoxRules() {
        if (_comboBoxRules.isEmpty()) {
            _comboBoxRules << new __ComboBoxNotEmptyRule();
        }
        return _comboBoxRules;
    }

    static ValidityRules lineEditRules() {
        if (_lineEditRules.isEmpty()) {
            _lineEditRules << new __LineEditNotEmptyRule();
        }
        return _lineEditRules;
    }
}

#endif // FORMVALIDATORRULES_H
