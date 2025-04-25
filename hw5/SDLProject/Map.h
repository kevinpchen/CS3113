#pragma once
class ShaderProgram;

class Map {
public:
    explicit Map(const char *texture_path);   
    void Render(ShaderProgram &program);      

private:
    const char *m_path;
    unsigned    m_tex = 0;
};
