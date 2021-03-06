/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   gl_text_renderer_item.h                                  *
 *  GROUP:  Range                                                    *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   14-th March 2016                                         *
 *                                                                   *
 *  DESCRIPTION: OpenGL text renderer item class declaration         *
 *********************************************************************/

#ifndef __GL_TEXT_RENDERER_ITEM_H__
#define __GL_TEXT_RENDERER_ITEM_H__

#include <QColor>
#include <QFont>
#include <QPointF>

class GLTextRendererItem
{

    protected:

        //! Text color.
        QColor color;
        //! Font.
        QFont font;
        //! Position.
        QPointF position;
        //! Text.
        QString text;

    private:

        //! Internal initialization function.
        void _init(const GLTextRendererItem *pGlTextRendererItem = 0);

    public:

        //! Constructor.
        GLTextRendererItem(const QColor &color, const QFont &font, const QPointF &position, const QString &text);

        //! Copy constructor.
        GLTextRendererItem(const GLTextRendererItem &glTextRendererItem);

        //! Destructor.
        ~GLTextRendererItem();

        //! Assignment operator.
        GLTextRendererItem &operator =(const GLTextRendererItem &glTextRendererItem);

        friend class GLTextRenderer;

};

#endif // __GL_TEXT_RENDERER_ITEM_H__
