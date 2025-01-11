#pragma once

class Object
{
    virtual bool IsEditorOnly() const
    {
        return false;
    }
};