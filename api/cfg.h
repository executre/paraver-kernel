/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                               libparaver-api                              *
 *                      API Library for libparaver-kernel                    *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/


#ifndef CFG_H_INCLUDED
#define CFG_H_INCLUDED

#include <map>
#include <vector>
#include <sstream>
#include <string>
#include "paraverkerneltypes.h"

class KernelConnection;
class Window;
class Trace;
class Histogram;

TWindowLevel stringToLevel( const std::string& strLevel );

class TagFunction
{
  public:
    TagFunction()
    {}

    virtual ~TagFunction()
    {}

    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms )
    {
      return false;
    }

    virtual void setCurrentNameTag( const std::string & whichCurrentNameTag )
    {
      currentNameTag = whichCurrentNameTag;
    }

  protected:
    static std::string currentNameTag; // because windows are created after name tag
    static bool isWindowTag;
 };

struct SaveOptions
{
public:
  SaveOptions()
  {
    windowComputeYMaxOnLoad = false;
    windowScaleRelative = true;
    windowBeginTimeRelative = true;
    histoComputeGradient = true;
    histoAllTrace = true;
    enabledCFG4DMode = false;
  }

  std::string description;
  bool windowComputeYMaxOnLoad;
  bool windowScaleRelative;
  bool windowBeginTimeRelative;
  bool histoComputeGradient;
  bool histoAllTrace;
  bool enabledCFG4DMode;
};


class CFGLoader
{
  private:
    static std::map<std::string, TagFunction *> cfgTagFunctions;
    static void loadMap();
    static void unLoadMap();
    static bool firstMapLoaded;
    static void pushbackWindow( Window *whichWindow,
                                std::vector<Window *>& allWindows );
    static void pushbackAllWindows( const std::vector<Window *>& selected,
                                    const std::vector<Histogram *>& selectedHistos,
                                    std::vector<Window *>& allWindows );

  public:
    static bool hasCFGExtension( const std::string& filename );
    static bool isCFGFile( const std::string& filename );
    static bool isDimemasCFGFile( const std::string& filename ); // TODO: here?
    static bool loadDescription( const std::string& filename, std::string& description );

    static bool loadCFG( KernelConnection *whichKernel,
                         const std::string& filename,
                         Trace *whichTrace,
                         std::vector<Window *>& windows,
                         std::vector<Histogram *>& histograms,
                         SaveOptions &options );
    static bool saveCFG( const std::string& filename,
                         const SaveOptions& options,
                         const std::vector<Window *>& windows,
                         const std::vector<Histogram *>& histograms );
    static int findWindow( const Window *whichWindow,
                           const std::vector<Window *>& allWindows );
    static int findWindowBackwards( const Window *whichWindow,
                                    const std::vector<Window *>& allWindows,
                                    const std::vector<Window *>::const_iterator it );
    // CFG4D
    static const std::vector< std::string > getTagCFGFullList( Window *whichWindow );
    static const std::vector< std::string > getTagCFGFullList( Histogram *whichHistogram );

    static std::string errorLine;
};


class WindowName: public TagFunction
{
  public:
    WindowName()
    {}

    virtual ~WindowName()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;
};


class WindowType: public TagFunction
{
  public:
    WindowType()
    {}

    virtual ~WindowType()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFactors: public TagFunction
{
  public:
    WindowFactors()
    {}

    virtual ~WindowFactors()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowPositionX: public TagFunction
{
  public:
    WindowPositionX()
    {}

    virtual ~WindowPositionX()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowPositionY: public TagFunction
{
  public:
    WindowPositionY()
    {}

    virtual ~WindowPositionY()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowWidth: public TagFunction
{
  public:
    WindowWidth()
    {}

    virtual ~WindowWidth()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowHeight: public TagFunction
{
  public:
    WindowHeight()
    {}

    virtual ~WindowHeight()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowCommLines: public TagFunction
{
  public:
    WindowCommLines()
    {}

    virtual ~WindowCommLines()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFlagsEnabled: public TagFunction
{
  public:
    WindowFlagsEnabled()
    {}

    virtual ~WindowFlagsEnabled()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowNonColorMode: public TagFunction
{
  public:
    WindowNonColorMode()
    {}

    virtual ~WindowNonColorMode()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowColorMode: public TagFunction
{
  public:
    WindowColorMode()
    {}

    virtual ~WindowColorMode()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowCustomColorEnabled: public TagFunction
{
  public:
    WindowCustomColorEnabled()
    {}

    virtual ~WindowCustomColorEnabled()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class WindowCustomColorPalette: public TagFunction
{
  public:
    WindowCustomColorPalette()
    {}

    virtual ~WindowCustomColorPalette()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class WindowSemanticScaleMinAtZero: public TagFunction
{
  public:
    WindowSemanticScaleMinAtZero()
    {}

    virtual ~WindowSemanticScaleMinAtZero()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class WindowUnits: public TagFunction
{
  public:
    WindowUnits()
    {}

    virtual ~WindowUnits()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowOperation: public TagFunction
{
  public:
    WindowOperation()
    {}

    virtual ~WindowOperation()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowMaximumY: public TagFunction
{
  public:
    WindowMaximumY()
    {}

    virtual ~WindowMaximumY()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowMinimumY: public TagFunction
{
  public:
    WindowMinimumY()
    {}

    virtual ~WindowMinimumY()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowComputeYMax: public TagFunction
{
  public:
    WindowComputeYMax()
    {}

    virtual ~WindowComputeYMax()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowLevel: public TagFunction
{
  public:
    WindowLevel()
    {}

    virtual ~WindowLevel()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowZoomObjects: public TagFunction
{
  public:
    WindowZoomObjects()
    {}

    virtual ~WindowZoomObjects()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowIdentifiers: public TagFunction
{
  public:
    WindowIdentifiers()
    {}

    virtual ~WindowIdentifiers()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>& allWindows,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowScaleRelative: public TagFunction
{
  public:
    WindowScaleRelative()
    {}

    virtual ~WindowScaleRelative()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowEndTimeRelative: public TagFunction
{
  public:
    WindowEndTimeRelative()
    {}

    virtual ~WindowEndTimeRelative()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowObject: public TagFunction
{
  public:
    WindowObject()
    {}

    virtual ~WindowObject()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowBeginTime: public TagFunction
{
  public:
    WindowBeginTime()
    {}

    virtual ~WindowBeginTime()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowEndTime: public TagFunction
{
  public:
    WindowEndTime()
    {}

    virtual ~WindowEndTime()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowStopTime: public TagFunction
{
  public:
    WindowStopTime()
    {}

    virtual ~WindowStopTime()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowBeginTimeRelative: public TagFunction
{
  public:
    WindowBeginTimeRelative()
    {}

    virtual ~WindowBeginTimeRelative()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowNumberOfRow: public TagFunction
{
  public:
    WindowNumberOfRow()
    {}

    virtual ~WindowNumberOfRow()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class WindowSelectedFunctions: public TagFunction
{
  public:
    WindowSelectedFunctions()
    {}

    virtual ~WindowSelectedFunctions()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowComposeFunctions: public TagFunction
{
  public:
    WindowComposeFunctions()
    {}

    virtual ~WindowComposeFunctions()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowSemanticModule: public TagFunction
{
  public:
    WindowSemanticModule()
    {}

    virtual ~WindowSemanticModule()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFilterModule: public TagFunction
{
  public:
    WindowFilterModule()
    {}

    virtual ~WindowFilterModule()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFilterLogical: public TagFunction
{
  public:
    WindowFilterLogical()
    {}

    virtual ~WindowFilterLogical()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFilterPhysical: public TagFunction
{
  public:
    WindowFilterPhysical()
    {}

    virtual ~WindowFilterPhysical()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFilterBoolOpFromTo: public TagFunction
{
  public:
    WindowFilterBoolOpFromTo()
    {}

    virtual ~WindowFilterBoolOpFromTo()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFilterBoolOpTagSize: public TagFunction
{
  public:
    WindowFilterBoolOpTagSize()
    {}

    virtual ~WindowFilterBoolOpTagSize()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowFilterBoolOpTypeVal: public TagFunction
{
  public:
    WindowFilterBoolOpTypeVal()
    {}

    virtual ~WindowFilterBoolOpTypeVal()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowOpen: public TagFunction
{
  public:
    WindowOpen()
    {}

    virtual ~WindowOpen()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowDrawMode: public TagFunction
{
  public:
    WindowDrawMode()
    {}

    virtual ~WindowDrawMode()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowDrawModeRows: public TagFunction
{
  public:
    WindowDrawModeRows()
    {}

    virtual ~WindowDrawModeRows()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class WindowPixelSize: public TagFunction
{
  public:
    WindowPixelSize()
    {}

    virtual ~WindowPixelSize()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class WindowLabelsToDraw: public TagFunction
{
  public:
    WindowLabelsToDraw()
    {}

    virtual ~WindowLabelsToDraw()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class WindowPunctualColorWindow: public TagFunction
{
  public:
    WindowPunctualColorWindow()
    {}

    virtual ~WindowPunctualColorWindow()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>& allWindows,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class WindowSynchronize: public TagFunction
{
  public:
    WindowSynchronize()
    {}

    virtual ~WindowSynchronize()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DCreate: public TagFunction
{
  public:
    Analyzer2DCreate()
    {}

    virtual ~Analyzer2DCreate()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DName: public TagFunction
{
  public:
    Analyzer2DName()
    {}

    virtual ~Analyzer2DName()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DX: public TagFunction
{
  public:
    Analyzer2DX()
    {}

    virtual ~Analyzer2DX()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DY: public TagFunction
{
  public:
    Analyzer2DY()
    {}

    virtual ~Analyzer2DY()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DWidth: public TagFunction
{
  public:
    Analyzer2DWidth()
    {}

    virtual ~Analyzer2DWidth()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DHeight: public TagFunction
{
  public:
    Analyzer2DHeight()
    {}

    virtual ~Analyzer2DHeight()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DControlWindow: public TagFunction
{
  public:
    Analyzer2DControlWindow()
    {}

    virtual ~Analyzer2DControlWindow()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>& allWindows,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DDataWindow: public TagFunction
{
  public:
    Analyzer2DDataWindow()
    {}

    virtual ~Analyzer2DDataWindow()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>& allWindows,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DStatistic: public TagFunction
{
  public:
    Analyzer2DStatistic()
    {}

    virtual ~Analyzer2DStatistic()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DCalculateAll: public TagFunction
{
  public:
    Analyzer2DCalculateAll()
    {}

    virtual ~Analyzer2DCalculateAll()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DHideColumns: public TagFunction
{
  public:
    Analyzer2DHideColumns()
    {}

    virtual ~Analyzer2DHideColumns()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DHorizontal: public TagFunction
{
  public:
    Analyzer2DHorizontal()
    {}

    virtual ~Analyzer2DHorizontal()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DColor: public TagFunction
{
  public:
    Analyzer2DColor()
    {}

    virtual ~Analyzer2DColor()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DSemanticColor: public TagFunction
{
  public:
    Analyzer2DSemanticColor()
    {}

    virtual ~Analyzer2DSemanticColor()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DZoom: public TagFunction
{
  public:
    Analyzer2DZoom()
    {}

    virtual ~Analyzer2DZoom()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DAccumulator: public TagFunction
{
  public:
    Analyzer2DAccumulator()
    {}

    virtual ~Analyzer2DAccumulator()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DAccumulateByControlWindow: public TagFunction
{
  public:
    Analyzer2DAccumulateByControlWindow()
    {}

    virtual ~Analyzer2DAccumulateByControlWindow()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DSortCols: public TagFunction
{
  public:
    Analyzer2DSortCols()
    {}

    virtual ~Analyzer2DSortCols()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DSortCriteria: public TagFunction
{
  public:
    Analyzer2DSortCriteria()
    {}

    virtual ~Analyzer2DSortCriteria()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DSortReverse: public TagFunction
{
  public:
    Analyzer2DSortReverse()
    {}

    virtual ~Analyzer2DSortReverse()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DParameters: public TagFunction
{
  public:
    Analyzer2DParameters()
    {}

    virtual ~Analyzer2DParameters()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DAnalysisLimits: public TagFunction
{
  public:
    Analyzer2DAnalysisLimits()
    {}

    virtual ~Analyzer2DAnalysisLimits()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DRelativeTime: public TagFunction
{
  public:
    Analyzer2DRelativeTime()
    {}

    virtual ~Analyzer2DRelativeTime()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DComputeYScale: public TagFunction
{
  public:
    Analyzer2DComputeYScale()
    {}

    virtual ~Analyzer2DComputeYScale()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class Analyzer2DComputeYScaleZero: public TagFunction
{
  public:
    Analyzer2DComputeYScaleZero()
    {}

    virtual ~Analyzer2DComputeYScaleZero()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class Analyzer2DMinimum: public TagFunction
{
  public:
    Analyzer2DMinimum()
    {}

    virtual ~Analyzer2DMinimum()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DMaximum: public TagFunction
{
  public:
    Analyzer2DMaximum()
    {}

    virtual ~Analyzer2DMaximum()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DDelta: public TagFunction
{
  public:
    Analyzer2DDelta()
    {}

    virtual ~Analyzer2DDelta()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DComputeGradient: public TagFunction
{
  public:
    Analyzer2DComputeGradient()
    {}

    virtual ~Analyzer2DComputeGradient()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class Analyzer2DMinimumGradient: public TagFunction
{
  public:
    Analyzer2DMinimumGradient()
    {}

    virtual ~Analyzer2DMinimumGradient()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DMaximumGradient: public TagFunction
{
  public:
    Analyzer2DMaximumGradient()
    {}

    virtual ~Analyzer2DMaximumGradient()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;
};


class Analyzer2DObjects: public TagFunction
{
  public:
    Analyzer2DObjects()
    {}

    virtual ~Analyzer2DObjects()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;
};


class Analyzer2DDrawModeObjects: public TagFunction
{
  public:
    Analyzer2DDrawModeObjects()
    {}

    virtual ~Analyzer2DDrawModeObjects()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;
};


class Analyzer2DDrawModeColumns: public TagFunction
{
  public:
    Analyzer2DDrawModeColumns()
    {}

    virtual ~Analyzer2DDrawModeColumns()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;
};


class Analyzer2DPixelSize: public TagFunction
{
  public:
    Analyzer2DPixelSize()
    {}

    virtual ~Analyzer2DPixelSize()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;
};

// DEPRECATED
class Analyzer2DCodeColor: public TagFunction
{
  public:
    Analyzer2DCodeColor()
    {}

    virtual ~Analyzer2DCodeColor()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DColorMode: public TagFunction
{
  public:
    Analyzer2DColorMode()
    {}

    virtual ~Analyzer2DColorMode()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DOnlyTotals: public TagFunction
{
  public:
    Analyzer2DOnlyTotals()
    {}

    virtual ~Analyzer2DOnlyTotals()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer2DShortLabels: public TagFunction
{
  public:
    Analyzer2DShortLabels()
    {}

    virtual ~Analyzer2DShortLabels()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};

class Analyzer2DSynchronize: public TagFunction
{
  public:
    Analyzer2DSynchronize()
    {}

    virtual ~Analyzer2DSynchronize()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer3DControlWindow: public TagFunction
{
  public:
    Analyzer3DControlWindow()
    {}

    virtual ~Analyzer3DControlWindow()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Window *>& allWindows,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer3DComputeYScale: public TagFunction
{
  public:
    Analyzer3DComputeYScale()
    {}

    virtual ~Analyzer3DComputeYScale()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const SaveOptions& options,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer3DMinimum: public TagFunction
{
  public:
    Analyzer3DMinimum()
    {}

    virtual ~Analyzer3DMinimum()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer3DMaximum: public TagFunction
{
  public:
    Analyzer3DMaximum()
    {}

    virtual ~Analyzer3DMaximum()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer3DDelta: public TagFunction
{
  public:
    Analyzer3DDelta()
    {}

    virtual ~Analyzer3DDelta()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class Analyzer3DFixedValue: public TagFunction
{
  public:
    Analyzer3DFixedValue()
    {}

    virtual ~Analyzer3DFixedValue()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );
    static void printLine( std::ofstream& cfgFile,
                           const std::vector<Histogram *>::const_iterator it );

    static const std::string &getTagCFG() { return tagCFG; }


  protected:
    static std::string tagCFG;

};


class TagAliasCFG4D: public TagFunction
{
  public:
    TagAliasCFG4D()
    {}

    virtual ~TagAliasCFG4D()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );

    static const std::string &getTagCFG() { return tagCFG; }

    static void printAliasList( std::ofstream& cfgFile,
                                const std::vector<Window *>::const_iterator it );
    static void printAliasList( std::ofstream& cfgFile,
                                const std::vector<Histogram *>::const_iterator it );

  protected:
    static std::string tagCFG;
};



class TagAliasStatisticCFG4D: public TagFunction
{
  public:
    TagAliasStatisticCFG4D()
    {}

    virtual ~TagAliasStatisticCFG4D()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );

    static const std::string &getTagCFG() { return tagCFG; }

    static void printAliasList( std::ofstream& cfgFile,
                                const std::vector<Histogram *>::const_iterator it );

  protected:
    static std::string tagCFG;
};


class TagAliasParamCFG4D: public TagFunction
{
  public:
    TagAliasParamCFG4D()
    {}

    virtual ~TagAliasParamCFG4D()
    {}
    virtual bool parseLine( KernelConnection *whichKernel, std::istringstream& line,
                            Trace *whichTrace,
                            std::vector<Window *>& windows,
                            std::vector<Histogram *>& histograms );

    static const std::string &getTagCFG() { return tagCFG; }

    static void printAliasList( std::ofstream& cfgFile,
                                const std::vector<Window *>::const_iterator it );

  protected:
    static std::string tagCFG;
};

#endif // CFG_H_INCLUDED
