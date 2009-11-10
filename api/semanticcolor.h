#ifndef SEMANTICCOLOR_H_INCLUDED
#define SEMANTICCOLOR_H_INCLUDED

#include "paravertypes.h"
#include "paraverkerneltypes.h"

class Window;

class SemanticColor
{
  public:
    enum TColorFunction
    {
      COLOR = 0,
      GRADIENT,
      NOT_NULL_GRADIENT
    };

    static rgb BACKGROUND;

    static UINT32 getNumColors();
    static rgb* getCodeColors();
    static rgb getBeginGradientColor();
    static rgb getEndGradientColor();
    static rgb getAboveOutlierColor();
    static rgb getBelowOutlierColor();

    virtual ~SemanticColor()
    {};

    virtual rgb calcColor( TSemanticValue whichValue,
                           TSemanticValue minimum,
                           TSemanticValue maximum ) const = 0;

  private:
    static UINT32 numColors;
    static rgb codeColor[];

    static rgb beginGradientColor;
    static rgb endGradientColor;
    static rgb aboveOutlierColor;
    static rgb belowOutlierColor;
};

class CodeColor: public SemanticColor
{
  public:
    CodeColor();
    ~CodeColor();

    UINT32 getNumColors() const;
    rgb getColor( UINT32 pos ) const;
    void setColor( UINT32 pos, rgb color );
    void addColor( rgb color );
    rgb calcColor( TSemanticValue whichValue,
                   TSemanticValue minimum,
                   TSemanticValue maximum ) const;

  private:
    vector<rgb> colors;
};


class GradientColor: public SemanticColor
{
  public:
    enum TGradientFunction
    {
      LINEAR = 0,
      STEPS,
      LOGARITHMIC,
      EXPONENTIAL
    };

    GradientColor();
    ~GradientColor();

    void setBeginGradientColor( rgb color );
    rgb getBeginGradientColor() const;

    void setEndGradientColor( rgb color );
    rgb getEndGradientColor() const;

    void setAboveOutlierColor( rgb color );
    rgb getAboveOutlierColor() const;

    void setBelowOutlierColor( rgb color );
    rgb getBelowOutlierColor() const;

    void allowOutliers( bool activate );
    bool getAllowOutliers() const;
    void allowOutOfScale( bool activate );
    bool getAllowOutOfScale() const;

    TGradientFunction getGradientFunction() const;
    void setGradientFunction( TGradientFunction whichFunction );
    INT16 getNumSteps() const;
    void setNumSteps( INT16 steps );

    rgb calcColor( TSemanticValue whichValue,
                   TSemanticValue minimum,
                   TSemanticValue maximum ) const;

    void copy( GradientColor &destiny );

  private:
    bool drawOutlier;
    bool drawOutOfScale;

    rgb beginGradientColor;
    rgb endGradientColor;
    rgb aboveOutlierColor;
    rgb belowOutlierColor;

    double redStep;
    double greenStep;
    double blueStep;

    TGradientFunction function;
    INT16 numSteps;

    void recalcSteps();

    rgb functionLinear( TSemanticValue whichValue,
                        TSemanticValue minimum,
                        TSemanticValue maximum ) const;
    rgb functionSteps( TSemanticValue whichValue,
                       TSemanticValue minimum,
                       TSemanticValue maximum ) const;
    rgb functionLog( TSemanticValue whichValue,
                     TSemanticValue minimum,
                     TSemanticValue maximum ) const;
    rgb functionExp( TSemanticValue whichValue,
                     TSemanticValue minimum,
                     TSemanticValue maximum ) const;

};

#endif // SEMANTICCOLOR_H_INCLUDED
