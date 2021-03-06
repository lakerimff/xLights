#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <string>

class wxXmlNode;
class BaseObject;

class ObjectManager
{
public:
    ObjectManager();
    virtual ~ObjectManager();

    std::string GenerateObjectName(const std::string& candidateName) const;

    virtual BaseObject *GetObject(const std::string &name) const = 0;

protected:
    wxXmlNode *modelNode;
    wxXmlNode *groupNode;

private:

};

#endif // OBJECTMANAGER_H
