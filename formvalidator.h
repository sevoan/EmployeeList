#ifndef INPUTVIEWVALIDATOR_H
#define INPUTVIEWVALIDATOR_H

#include <QObject>
#include <QVector>

class QWidget;
class QSpinBox;
class QLineEdit;
class QComboBox;

class ValidityRule {
public:
    virtual bool isValid(const QWidget* widget) const = 0;
};

typedef QVector<ValidityRule*> ValidityRules;

class FormValidatorPrivate;

// TODO: rename to FormValidator
class FormValidator : public QObject
{
    Q_OBJECT
public:
    FormValidator();
    ~FormValidator();

    bool isValidationEnabled() const;
    void setAutoFocusEnabled(bool enabled);
    bool isAutoFocusEnabled() const;

    void appendView(QSpinBox* w, ValidityRules rules);
    void appendView(QLineEdit* w, ValidityRules rules);
    void appendView(QComboBox* w, ValidityRules rules);

    bool validate(QWidget* widget);

public Q_SLOTS:
    bool validateAll();
    void stopValidation();
    void startValidation();

private:
    FormValidatorPrivate* d_ptr;
    Q_DECLARE_PRIVATE(FormValidator)
    Q_DISABLE_COPY(FormValidator)
};

#endif // INPUTVIEWVALIDATOR_H
