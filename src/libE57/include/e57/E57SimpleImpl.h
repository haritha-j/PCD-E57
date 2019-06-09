//////////////////////////////////////////////////////////////////////////
//
//  E57SimpleImpl.h - private implementation header of E57 format reference implementation.
//
//	Copyright (c) 2010 Stan Coleby (scoleby@intelisum.com)
//	All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person or organization
//  obtaining a copy of the software and accompanying documentation covered by
//  this license (the "Software") to use, reproduce, display, distribute,
//  execute, and transmit the Software, and to prepare derivative works of the
//  Software, and to permit third-parties to whom the Software is furnished to
//  do so, all subject to the following:
// 
//  The copyright notices in the Software and this entire statement, including
//  the above license grant, this restriction and the following disclaimer,
//  must be included in all copies of the Software, in whole or in part, and
//  all derivative works of the Software, unless such copies or derivative
//  works are solely in the form of machine-executable object code generated by
//  a source language processor.
// 
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
//  FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//	The Boost license Vestion 1.0 - August 17th, 2003 is discussed in
//	http://www.boost.org/users/license.html.
//
//  This source code is only intended as a supplement to promote the
//	ASTM E57.04 3D Imaging System File Format standard for interoperability
//	of Lidar Data.  See http://www.libe57.org.
//
//////////////////////////////////////////////////////////////////////////
//
//	New E57SimpleImpl.h
//	V1	May 18, 2010	Stan Coleby		scoleby@intelisum.com
//  V6	June 8, 2010	Stan Coleby		scoleby@intelisum.com
//
//////////////////////////////////////////////////////////////////////////

//! @file E57SimpleImpl.h

#ifndef E57SIMPLEIMPL_H_INCLUDED
#define E57SIMPLEIMPL_H_INCLUDED

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/crc.hpp>        // for boost::crc_optimal

// Define the following symbol adds some functions to the API for implementation purposes.
// These functions are not available to a normal API user.
#define E57_INTERNAL_IMPLEMENTATION_ENABLE 1

#ifndef E57SIMPLE_H_INCLUDED
#  include "e57/E57Simple.h"
#endif

namespace e57 {

char *	GetNewGuid(void);
double	GetGPSTime(void);

double	GetGPSDateTimeFromUTC(
	int utc_year,		//!< The year 1900-9999
	int utc_month,		//!< The month 1-12
	int utc_day,		//!< The day 1-31
	int utc_hour,		//!< The hour 0-23
	int utc_minute,		//!< The minute 0-59
	float utc_seconds	//!< The seconds 0.0 - 59.999
	);

void	GetUTCFromGPSDateTime(
    double gpsTime,		//!< GPS Date Time
	int &utc_year,		//!< The year 1900-9999
	int &utc_month,		//!< The month 1-12
	int &utc_day,		//!< The day 1-31
	int &utc_hour,		//!< The hour 0-23
	int &utc_minute,		//!< The minute 0-59
	float &utc_seconds	//!< The seconds 0.0 - 59.999
	);
#if defined(WIN32)
double	GetGPSDateTimeFromSystemTime(
	SYSTEMTIME	sysTim		//!< Windows System Time
	);
void	GetSystemTimeFromGPSDateTime(
	double		gpsTime,	//!< GPS Date Time
	SYSTEMTIME	&sysTim		//!< Windows System Time
	);
#endif

////////////////////////////////////////////////////////////////////
//
//	e57::ReaderImpl
//

//! This is the E57 Reader class

class	ReaderImpl : public boost::enable_shared_from_this<ReaderImpl> {

private:

	ImageFile		imf_;
	StructureNode	root_;

	VectorNode		data3D_;

	VectorNode		images2D_;

public:

//! This function is the constructor for the reader class
					ReaderImpl(
						const ustring & filePath		//!< file path string
						);

//! This function is the destructor for the reader class
virtual				~ReaderImpl(void);

//! This function returns true if the file is open
virtual	bool		IsOpen(void);

//! This function closes the file
virtual	bool		Close(void);

////////////////////////////////////////////////////////////////////
//
//	File information
//
//! This function returns the file header information
virtual bool		GetE57Root(
						E57Root & fileHeader	//!< This is the main header information
					    );	//!< /return Returns true if sucessful

////////////////////////////////////////////////////////////////////
//
//	Camera Image picture data
//
//! This function returns the total number of Picture Blocks
virtual	int32_t		GetImage2DCount( void);

//! This function returns the Image2Ds header and positions the cursor
virtual bool		ReadImage2D( 
						int32_t				imageIndex,		//!< This in the index into the Image2Ds vector
						Image2D &			Image2DHeader	//!< pointer to the Image2D structure to receive the picture information
						);						//!< /return Returns true if sucessful

//! This function returns the size of the image data
virtual bool		GetImage2DSizes(
						int32_t					imageIndex,		//!< This in the index into the image2D vector
						e57::Image2DProjection&	imageProjection,//!< identifies the projection desired.
						e57::Image2DType &		imageType,		//!< identifies the image format desired.
						int64_t &				imageWidth,		//!< The image width (in pixels).
						int64_t &				imageHeight,	//!< The image height (in pixels).
						int64_t &				imageSize,		//!< This is the total number of bytes for the image blob.
						e57::Image2DType &		imageMaskType,	//!< This is E57_PNG_IMAGE_MASK if "imageMask" is defined in the projection
						e57::Image2DType &		imageVisualType	//!< This is image type of the VisualReferenceRepresentation if given.
						);										//!< /return Returns true if sucessful

//! This function reads the block
virtual	int64_t		ReadImage2DData(
						int32_t					imageIndex,		//!< picture block index
						e57::Image2DProjection	imageProjection,//!< identifies the projection desired.
						e57::Image2DType		imageType,		//!< identifies the image format desired.
						void *					pBuffer,		//!< pointer the buffer
						int64_t					start,			//!< position in the block to start reading
						int64_t					count			//!< size of desired chuck or buffer size
						);										//!< /return Returns the number of bytes transferred.

//! This function reads one of the image blobs
virtual bool		GetImage2DNodeSizes(
						e57::StructureNode		image,			//!< 1 of 3 projects or the visual
						e57::Image2DType &		imageType,		//!< identifies the image format desired.
						int64_t &				imageWidth,		//!< The image width (in pixels).
						int64_t &				imageHeight,	//!< The image height (in pixels).
						int64_t &				imageSize,		//!< This is the total number of bytes for the image blob.
						e57::Image2DType &		imageMaskType	//!< This is E57_PNG_IMAGE_MASK if "imageMask" is defined in the projection
						);										//!< /return Returns true if sucessful

virtual int64_t		ReadImage2DNode(
						e57::StructureNode		image,			//!< 1 of 3 projects or the visual
						e57::Image2DType		imageType,		//!< identifies the image format desired.
						void *					pBuffer,		//!< pointer the buffer
						int64_t					start,			//!< position in the block to start reading
						int64_t					count			//!< size of desired chuck or buffer size
						);

////////////////////////////////////////////////////////////////////
//
//	Scanner Image 3d data
//
//! This function returns the total number of Image Blocks
virtual	int32_t		GetData3DCount( void);

//! This function returns the Data3D header and positions the cursor
virtual bool		ReadData3D( 
						int32_t		dataIndex,	//!< This in the index into the images3D vector
						Data3D &	data3DHeader //!< pointer to the Data3D structure to receive the image information
						);	//!< /return Returns true if sucessful

//! This function returns the size of the point data
virtual	bool		GetData3DSizes(
						int32_t		dataIndex,	//!< This in the index into the images3D vector
						int64_t &	rowMax,		//!< This is the maximum row size
						int64_t &	columnMax,	//!< This is the maximum column size
						int64_t &	pointsSize,	//!< This is the total number of point records
						int64_t &	groupsSize,	//!< This is the total number of group reocrds
						int64_t &	countSize,	//!< This is the maximum point count per group
						bool &		bColumnIndex	//!< This indicates that the idElementName is "columnIndex"
						);

//! This funtion writes out the group data
virtual bool		ReadData3DGroupsData(
						int32_t		dataIndex,			//!< data block index given by the NewData3D
						int32_t		groupCount,			//!< size of each of the buffers given
						int64_t*	idElementValue,		//!< index for this group
						int64_t*	startPointIndex,	//!< Starting index in to the "points" data vector for the groups
						int64_t*	pointCount			//!< size of the groups given
						);								//!< \return Return true if sucessful, false otherwise

//! This function sets up the point data fields 
/* All the non-NULL buffers in the call below have number of elements = pointCount.
Call the CompressedVectorReader::read() until all data is read.
*/

virtual CompressedVectorReader	SetUpData3DPointsData(
						int32_t		dataIndex,			//!< data block index given by the NewData3D
						int64_t		pointCount,			//!< size of each element buffer.
						double*		cartesianX,			//!< pointer to a buffer with the X coordinate (in meters) of the point in Cartesian coordinates
						double*		cartesianY,			//!< pointer to a buffer with the Y coordinate (in meters) of the point in Cartesian coordinates
						double*		cartesianZ,			//!< pointer to a buffer with the Z coordinate (in meters) of the point in Cartesian coordinates
						int8_t*		cartesianInvalidState = NULL,	//!< Value = 0 if the point is considered valid, 1 otherwise

						double*		intensity = NULL,	//!< pointer to a buffer with the Point response intensity. Unit is unspecified
						int8_t*		isIntensityInvalid = NULL,	//!< Value = 0 if the intensity is considered valid, 1 otherwise

						uint16_t*	colorRed = NULL,	//!< pointer to a buffer with the Red color coefficient. Unit is unspecified
						uint16_t*	colorGreen = NULL,	//!< pointer to a buffer with the Green color coefficient. Unit is unspecified
						uint16_t*	colorBlue = NULL,	//!< pointer to a buffer with the Blue color coefficient. Unit is unspecified
						int8_t*		isColorInvalid = NULL,	//!< Value = 0 if the color is considered valid, 1 otherwise

						double*		sphericalRange = NULL,		//!< pointer to a buffer with the range (in meters) of points in spherical coordinates. Shall be non-negative
						double*		sphericalAzimuth = NULL,	//!< pointer to a buffer with the Azimuth angle (in radians) of point in spherical coordinates
						double*		sphericalElevation = NULL,	//!< pointer to a buffer with the Elevation angle (in radians) of point in spherical coordinates
						int8_t*		sphericalInvalidState = NULL, //!< Value = 0 if the range is considered valid, 1 otherwise

						int32_t*	rowIndex = NULL,	//!< pointer to a buffer with the row number of point (zero based). This is useful for data that is stored in a regular grid.Shall be in the interval (0, 2^63).
						int32_t*	columnIndex = NULL,	//!< pointer to a buffer with the column number of point (zero based). This is useful for data that is stored in a regular grid. Shall be in the interval (0, 2^63).
						int8_t*		returnIndex = NULL,	//!< pointer to a buffer with the number of this return (zero based). That is, 0 is the first return, 1 is the second, and so on. Shall be in the interval (0, returnCount). Only for multi-return sensors. 
						int8_t*		returnCount = NULL,	//!< pointer to a buffer with the total number of returns for the pulse that this corresponds to. Shall be in the interval (0, 2^63). Only for multi-return sensors. 

						double*		timeStamp = NULL,	//!< pointer to a buffer with the time (in seconds) since the start time for the data, which is given by acquisitionStart in the parent Data3D Structure. Shall be non-negative
						int8_t*		isTimeStampInvalid = NULL	//!< Value = 0 if the timeStamp is considered valid, 1 otherwise
						);

//! This function returns the file raw E57Root Structure Node
virtual	StructureNode		GetRawE57Root(void);	//!< /return Returns the E57Root StructureNode
//! This function returns the raw Data3D Vector Node
virtual VectorNode			GetRawData3D(void);		//!< /return Returns the raw Data3D VectorNode
//! This function returns the raw Image2D Vector Node
virtual VectorNode			GetRawImages2D(void);	//!< /return Returns the raw Image2D VectorNode

}; //end Reader class


////////////////////////////////////////////////////////////////////
//
//	e57::Writer
//

//! This is the E57 Writer class

class	WriterImpl : public boost::enable_shared_from_this<WriterImpl> {

private:
	ImageFile				imf_;
	StructureNode			root_;

	VectorNode				data3D_;

	VectorNode				images2D_;

public:

//! This function is the constructor for the writer class
					WriterImpl(
						const ustring & filePath,		//!< file path string
						const ustring & coordinateMetaData	//!< Information describing the Coordinate Reference System to be used for the file
						);

//! This function is the destructor for the writer class
virtual				~WriterImpl(void);

//! This function returns true if the file is open
virtual	bool		IsOpen(void);

//! This function closes the file
virtual	bool		Close(void);

////////////////////////////////////////////////////////////////////
//
//	Camera Image picture data
//

//! This function sets up the Image2Ds header and positions the cursor
//* The user needs to config a Image2D structure with all the camera information before making this call. */
virtual int32_t		NewImage2D( 
						Image2D &	image2DHeader	//!< pointer to the Image2D structure to receive the picture information
						);						//!< /return Returns the Image2D index

//! This function writes the block
virtual	int64_t		WriteImage2DData(
						int32_t					imageIndex,		//!< picture block index given by the NewImage2D
						e57::Image2DType		imageType,		//!< identifies the image format desired.
						e57::Image2DProjection	imageProjection,//!< identifies the projection desired.
						void *					pBuffer,		//!< pointer the buffer
						int64_t					start,			//!< position in the block to start writing
						int64_t					count			//!< size of desired chuck or buffer size
						);										//!< /return Returns the number of bytes written

//! This function writes the projection image
virtual int64_t		WriteImage2DNode(
						e57::StructureNode		image,			//!< 1 of 3 projects or the visual
						e57::Image2DType		imageType,		//!< identifies the image format desired.
						void *					pBuffer,		//!< pointer the buffer
						int64_t					start,			//!< position in the block to start reading
						int64_t					count			//!< size of desired chuck or buffer size
						);

//! This function sets up the Data3D header and positions the cursor for the binary data
//* The user needs to config a Data3D structure with all the scanning information before making this call. */

virtual int32_t		NewData3D( 
						Data3D &	data3DHeader	//!< pointer to the Data3D structure to receive the image information
						);							//!< /return Returns the index of the new scan's data3D block.

//! This function writes out blocks of point data
virtual CompressedVectorWriter	SetUpData3DPointsData(
						int32_t		dataIndex,			//!< data block index given by the NewData3D
						int64_t		pointCount,			//!< size of each of the buffers given
						double*		cartesianX,			//!< pointer to a buffer with the X coordinate (in meters) of the point in Cartesian coordinates
						double*		cartesianY,			//!< pointer to a buffer with the Y coordinate (in meters) of the point in Cartesian coordinates
						double*		cartesianZ,			//!< pointer to a buffer with the Z coordinate (in meters) of the point in Cartesian coordinates
						int8_t*		cartesianInvalidState = NULL,	//!< Value = 0 if the point is considered valid, 1 otherwise

						double*		intensity = NULL,	//!< pointer to a buffer with the Point response intensity. Unit is unspecified
						int8_t*		isIntensityInvalid = NULL,	//!< Value = 0 if the intensity is considered valid, 1 otherwise

						uint16_t*	colorRed = NULL,	//!< pointer to a buffer with the Red color coefficient. Unit is unspecified
						uint16_t*	colorGreen = NULL,	//!< pointer to a buffer with the Green color coefficient. Unit is unspecified
						uint16_t*	colorBlue = NULL,	//!< pointer to a buffer with the Blue color coefficient. Unit is unspecified
						int8_t*		isColorInvalid = NULL,	//!< Value = 0 if the color is considered valid, 1 otherwise

						double*		sphericalRange = NULL,		//!< pointer to a buffer with the range (in meters) of points in spherical coordinates. Shall be non-negative
						double*		sphericalAzimuth = NULL,	//!< pointer to a buffer with the Azimuth angle (in radians) of point in spherical coordinates
						double*		sphericalElevation = NULL,	//!< pointer to a buffer with the Elevation angle (in radians) of point in spherical coordinates
						int8_t*		sphericalInvalidState = NULL, //!< Value = 0 if the range is considered valid, 1 otherwise

						int32_t*	rowIndex = NULL,	//!< pointer to a buffer with the row number of point (zero based). This is useful for data that is stored in a regular grid.Shall be in the interval (0, 2^63).
						int32_t*	columnIndex = NULL,	//!< pointer to a buffer with the column number of point (zero based). This is useful for data that is stored in a regular grid. Shall be in the interval (0, 2^63).
						int8_t*		returnIndex = NULL,	//!< pointer to a buffer with the number of this return (zero based). That is, 0 is the first return, 1 is the second, and so on. Shall be in the interval (0, returnCount). Only for multi-return sensors. 
						int8_t*		returnCount = NULL,	//!< pointer to a buffer with the total number of returns for the pulse that this corresponds to. Shall be in the interval (0, 2^63). Only for multi-return sensors. 

						double*		timeStamp = NULL,	//!< pointer to a buffer with the time (in seconds) since the start time for the data, which is given by acquisitionStart in the parent Data3D Structure. Shall be non-negative
						int8_t*		isTimeStampInvalid = NULL	//!< Value = 0 if the timeStamp is considered valid, 1 otherwise
						);


//! This funtion writes out the group data
virtual bool		WriteData3DGroupsData(
						int32_t		dataIndex,			//!< data block index given by the NewData3D
						int32_t		groupCount,			//!< size of each of the buffers given
						int64_t*	idElementValue,		//!< index for this group
						int64_t*	startPointIndex,	//!< Starting index in to the "points" data vector for the groups
						int64_t*	pointCount			//!< size of the groups given
						);								//!< \return Return true if sucessful, false otherwise

//! This function returns the file raw E57Root Structure Node
virtual	StructureNode		GetRawE57Root(void);	//!< /return Returns the E57Root StructureNode
//! This function returns the raw Data3D Vector Node
virtual VectorNode			GetRawData3D(void);		//!< /return Returns the raw Data3D VectorNode
//! This function returns the raw Image2D Vector Node
virtual VectorNode			GetRawImages2D(void);	//!< /return Returns the raw Image2D VectorNode

}; //end Writer class

}; //end namespace

#endif
