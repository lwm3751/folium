#ifndef FOLIUM_IO_H
#define FOLIUM_IO_H

#include <string>
#include <boost/shared_ptr.hpp>

namespace folium
{
    using std::string;
    using boost::shared_ptr;

    class io
    {
    public:
        virtual bool readable()=0;
        virtual string readline()=0;
        virtual void writeline(const string&)=0;
        virtual ~io(){}
    protected:
        io(){}
    };
    shared_ptr<io> stdio();
    shared_ptr<io> openexe(const string& path);
}

#endif //FOLIUM_IO_H
