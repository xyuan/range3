/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   gl_text_renderer.h                                       *
 *  GROUP:  Range                                                    *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   14-th March 2016                                         *
 *                                                                   *
 *  DESCRIPTION: OpenGL text renderer class declaration              *
 *********************************************************************/

#ifndef __GL_TEXT_RENDERER_H__
#define __GL_TEXT_RENDERER_H__

#include <QList>
#include <QPainter>

#include "gl_text_renderer_item.h"

class GLTextRenderer
{

    protected:

        //! Stack of texts to render.
        QList<GLTextRendererItem> list;

    private:

        //! Internal initialization function.
        void _init(const GLTextRenderer *pGlTextRenderer = 0);

    public:

        //! Constructor.
        GLTextRenderer();

        //! Copy constructor.
        GLTextRenderer(const GLTextRenderer &glTextRenderer);

        //! Destructor.
        ~GLTextRenderer();

        //! Assignment operator.
        GLTextRenderer &operator =(const GLTextRenderer &glTextRenderer);

        //! Add item.
        void add(const GLTextRendererItem &item);

        //! Clear list.
        void clear(void);

        //! Render items.
        void render(QPainter &painter) const;

};

#endif // __GL_TEXT_RENDERER_H__
