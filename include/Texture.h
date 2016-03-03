#pragma once

class Texture {  
	protected:
	    
	    const char* filename;
	    
	    static unsigned char* loadPPM(const char*, int&, int&);
	    
	public:
	    
	    unsigned int id;
	    
	    Texture(void);
	    Texture(const char* filename);
	    virtual ~Texture(void);
	    
	    void bind(void);
	    void unbind(void);
    
};
