#ifndef USER_H
#define USER_H

#include <QString>

enum class UserRole {
    Client,
    Administrator
};

class User
{
public:
    User();
    User(int id, const QString& username, const QString& password, 
         const QString& fullName, UserRole role);
    
    // Getters
    int getId() const { return m_id; }
    QString getUsername() const { return m_username; }
    QString getPassword() const { return m_password; }
    QString getFullName() const { return m_fullName; }
    UserRole getRole() const { return m_role; }
    
    // Setters
    void setId(int id) { m_id = id; }
    void setUsername(const QString& username) { m_username = username; }
    void setPassword(const QString& password) { m_password = password; }
    void setFullName(const QString& fullName) { m_fullName = fullName; }
    void setRole(UserRole role) { m_role = role; }
    
    // Helpers
    bool isAdministrator() const { return m_role == UserRole::Administrator; }
    bool isClient() const { return m_role == UserRole::Client; }
    QString getRoleString() const;

private:
    int m_id;
    QString m_username;
    QString m_password;
    QString m_fullName;
    UserRole m_role;
};

#endif // USER_H

