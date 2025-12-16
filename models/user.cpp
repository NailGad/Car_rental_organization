#include "user.h"

User::User()
    : m_id(0), m_username(""), m_password(""), m_fullName(""), m_role(UserRole::Client)
{
}

User::User(int id, const QString& username, const QString& password, 
           const QString& fullName, UserRole role)
    : m_id(id), m_username(username), m_password(password), 
      m_fullName(fullName), m_role(role)
{
}

QString User::getRoleString() const
{
    return m_role == UserRole::Administrator ? "Администратор" : "Клиент";
}

