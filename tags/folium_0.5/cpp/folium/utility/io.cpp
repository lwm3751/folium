#include "eleeye/pipe.h"
#include "io.h"
#include <boost/utility.hpp>

namespace folium
{
    class io_implement: public io, private boost::noncopyable
    {
    public:
        bool open(const char* path);

        virtual bool readable();
        virtual string readline();
        virtual void writeline(const string&);
    private:
        PipeStruct m_pipe;
        string m_line;
    };
    bool io_implement::open(const char* path)
    {
        m_pipe.Open(path);
        return true;
    }

    bool io_implement::readable()
    {
        if (m_line.empty())
        {
            char buffer[LINE_INPUT_MAX_CHAR];
            if (m_pipe.LineInput(buffer))
                m_line = buffer;
        }
        return !m_line.empty();
    }
    string io_implement::readline()
    {
        if (m_line.empty())
        {
            char buffer[LINE_INPUT_MAX_CHAR];
            if (m_pipe.LineInput(buffer))
                m_line = buffer;
        }

        string line = m_line;
        m_line.clear();
        return line;
    }
    void io_implement::writeline(const string& line)
    {
        m_pipe.LineOutput(line.c_str());
    }

    boost::shared_ptr<io> stdio()
    {
        static boost::shared_ptr<io_implement> instance;
        if (NULL == instance.get())
        {
            instance = boost::shared_ptr<io_implement>(new io_implement());
            instance->open(NULL);
        }
        return instance;
    }

    boost::shared_ptr<io> openexe(const string& path)
    {
        boost::shared_ptr<io_implement> ioobject(new io_implement());
        if (ioobject->open(path.c_str()))
            return ioobject;
        return boost::shared_ptr<io>();
    }
}
