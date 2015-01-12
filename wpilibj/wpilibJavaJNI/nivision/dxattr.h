
IMAQdxError NI_FUNC IMAQdxGetAttributeU32(IMAQdxSession id, const char* name, uInt32* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeI64(IMAQdxSession id, const char* name, Int64* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeF64(IMAQdxSession id, const char* name, float64* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeString(IMAQdxSession id, const char* name, char value[IMAQDX_MAX_API_STRING_LENGTH]);
IMAQdxError NI_FUNC IMAQdxGetAttributeEnum(IMAQdxSession id, const char* name, IMAQdxEnumItem* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeBool(IMAQdxSession id, const char* name, bool32* value);

IMAQdxError NI_FUNC IMAQdxGetAttributeMinimumU32(IMAQdxSession id, const char* name, uInt32* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeMinimumI64(IMAQdxSession id, const char* name, Int64* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeMinimumF64(IMAQdxSession id, const char* name, float64* value);

IMAQdxError NI_FUNC IMAQdxGetAttributeMaximumU32(IMAQdxSession id, const char* name, uInt32* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeMaximumI64(IMAQdxSession id, const char* name, Int64* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeMaximumF64(IMAQdxSession id, const char* name, float64* value);

IMAQdxError NI_FUNC IMAQdxGetAttributeIncrementU32(IMAQdxSession id, const char* name, uInt32* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeIncrementI64(IMAQdxSession id, const char* name, Int64* value);
IMAQdxError NI_FUNC IMAQdxGetAttributeIncrementF64(IMAQdxSession id, const char* name, float64* value);

IMAQdxError NI_FUNC IMAQdxSetAttributeU32(IMAQdxSession id, const char* name, uInt32 value);
IMAQdxError NI_FUNC IMAQdxSetAttributeI64(IMAQdxSession id, const char* name, Int64 value);
IMAQdxError NI_FUNC IMAQdxSetAttributeF64(IMAQdxSession id, const char* name, float64 value);
IMAQdxError NI_FUNC IMAQdxSetAttributeString(IMAQdxSession id, const char* name, const char* value);
IMAQdxError NI_FUNC IMAQdxSetAttributeEnum(IMAQdxSession id, const char* name, const IMAQdxEnumItem* value);
IMAQdxError NI_FUNC IMAQdxSetAttributeBool(IMAQdxSession id, const char* name, bool32 value);

