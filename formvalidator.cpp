#include <QDebug>
#include <QWidget>
#include <QPalette>
#include <QStyle>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

#include "formvalidator_p.h"
#include "formvalidator.h"

#define VIEW_VALID      true
#define VIEW_INVALID    false

FormValidatorPrivate::FormValidatorPrivate(FormValidator* qq) :
    q_ptr(qq),
    isValidationEnabled(false),
    isAutoFocusEnabled(false)
{
}

FormValidatorPrivate::~FormValidatorPrivate()
{
    widgetRules.clear();
}

void FormValidatorPrivate::lineEditTextChanged(const QString&)
{
    Q_Q(FormValidator);
    QWidget* w = static_cast<QWidget*>(sender());
    q->validate(w);
}

void FormValidatorPrivate::spinBoxValueChanged(int)
{
    Q_Q(FormValidator);
    QWidget* w = static_cast<QWidget*>(sender());
    q->validate(w);
}

void FormValidatorPrivate::comboboxIndexChanged(int)
{
    Q_Q(FormValidator);
    QWidget* w = static_cast<QWidget*>(sender());
    q->validate(w);
}

FormValidator::FormValidator() :
    d_ptr(new FormValidatorPrivate(this))
{
}

FormValidator::~FormValidator()
{
    delete d_ptr;
}

bool FormValidator::isValidationEnabled() const
{
    Q_D(const FormValidator);
    return d->isValidationEnabled;
}

void FormValidator::setAutoFocusEnabled(bool enabled)
{
    Q_D(FormValidator);
    d->isAutoFocusEnabled = enabled;
}

bool FormValidator::isAutoFocusEnabled() const
{
    Q_D(const FormValidator);
    return d->isAutoFocusEnabled;
}

void FormValidator::appendView(QSpinBox* w, ValidityRules rules)
{
    Q_D(FormValidator);
    d->widgetRules.insert(w, rules);
    connect(w, static_cast<void(QSpinBox::*)(int value)>(&QSpinBox::valueChanged),
            d, &FormValidatorPrivate::spinBoxValueChanged);
}

void FormValidator::appendView(QLineEdit* w, ValidityRules rules)
{
    Q_D(FormValidator);
    d->widgetRules.insert(w, rules);
    connect(w, &QLineEdit::textChanged, d, &FormValidatorPrivate::lineEditTextChanged);
}

void FormValidator::appendView(QComboBox* w, ValidityRules rules)
{
    Q_D(FormValidator);
    d->widgetRules.insert(w, rules);
    connect(w, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            d, &FormValidatorPrivate::comboboxIndexChanged);
}

bool FormValidator::validate(QWidget* widget)
{
    Q_D(FormValidator);

    if (!d->isValidationEnabled)
        return VIEW_VALID;

    ValidityRules rules = d->widgetRules.value(widget);

    bool firstFocus = true;

    for (auto rule : rules) {
        bool valid = rule->isValid(widget);
        widget->setProperty("valid", valid);
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);

        if (!valid) {
            if (d->isAutoFocusEnabled && firstFocus) {
                widget->setFocus();
                firstFocus = false;
            }

            return VIEW_INVALID;
        }
    }

    return VIEW_VALID;
}

bool FormValidator::validateAll()
{
    Q_D(FormValidator);

    if (!d->isValidationEnabled) {
        qDebug() << "ValidateAll: but validation disabled";
        return VIEW_VALID;
    }

    bool valid = VIEW_VALID;

    // Use iterator because we want to have access to keys
    for (auto it = d->widgetRules.begin(); it != d->widgetRules.end(); ++it)
        valid &= validate(it.key());

    return valid;
}

void FormValidator::stopValidation()
{
    Q_D(FormValidator);
    d->isValidationEnabled = false;

    // Use iterator because we want to have access to keys
    for (auto it = d->widgetRules.begin(); it != d->widgetRules.end(); ++it) {
        auto widget = it.key();
        widget->setProperty("valid", true);
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    }
}

void FormValidator::startValidation()
{
    Q_D(FormValidator);
    d->isValidationEnabled = true;
}
