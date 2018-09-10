#pragma once

#include <osg/GraphicsContext>
#include <osgViewer/api/Win32/GraphicsHandleWin32>

namespace osgViewer
{

	class  CNoLeakPixelBufWin32 : public osg::GraphicsContext, public osgViewer::GraphicsHandleWin32
	{
	public:

		CNoLeakPixelBufWin32(osg::GraphicsContext::Traits* traits);

		virtual ~CNoLeakPixelBufWin32();

		virtual bool isSameKindAs(const Object* object) const { return dynamic_cast<const CNoLeakPixelBufWin32*>(object) != 0; }
		virtual const char* libraryName() const { return "osgViewer"; }
		virtual const char* className() const { return "PixelBufferWin32"; }

		virtual bool valid() const { return _valid; }

		/** Realize the GraphicsContext.*/
		virtual bool realizeImplementation();

		/** Return true if the graphics context has been realized and is ready to use.*/
		virtual bool isRealizedImplementation() const { return _realized; }

		/** Close the graphics context.*/
		virtual void closeImplementation();

		/** Make this graphics context current.*/
		virtual bool makeCurrentImplementation();
		virtual bool makeContextCurrentImplementation(GraphicsContext* /*readContext*/);

		/** Release the graphics context.*/
		virtual bool releaseContextImplementation();

		/** Swap the front and back buffers.*/
		virtual void swapBuffersImplementation();

		virtual void bindPBufferToTextureImplementation(GLenum /*buffer*/);

	protected:

		void init();

		bool            _initialized;
		bool            _valid;
		bool            _realized;
		int             _boundBuffer;
	};

}


