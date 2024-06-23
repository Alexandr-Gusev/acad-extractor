#include "core.h"
#include "utils.h"

static bool init_done = false;
static HRESULT init_hr;

static void init(void)
{
    if (!init_done)
    {
        init_hr = CoInitialize(0);
        init_done = true;
    }
    if (FAILED(init_hr)) THROW_HR(init_hr);
}

#ifdef AE_EARLY_BINDING

// not suitable for other versions
#import "acax24enu.tlb" no_namespace named_guids no_implementation raw_interfaces_only

AE_EXTERN_C long AE_DECL_SPEC select_on_screen
(
    std::vector<BSTR>* const names,
    std::vector<std::vector<std::pair<BSTR, BSTR>>>* const attrs_list,
    std::vector<std::vector<std::pair<BSTR, VARIANT>>>* const props_list,
    std::vector<BSTR>* const handles,
    std::function<void(int processed, int total)> callback,
    std::vector<std::vector<double>>* const points
)
{
#ifdef AE_EXPORT
    try
    {
#endif
    init();

    CLSID clsid;
    {
        HRESULT hr = CLSIDFromProgID(L"AutoCAD.Application", &clsid);
        if (FAILED(hr)) THROW_HR(hr);
    }

    IAcadApplication* Application;
    try
    {
        IUnknown* Application_;
        THROW_IF_FAILED(GetActiveObject(clsid, 0, &Application_));
        THROW_IF_FAILED(Application_->QueryInterface(IID_IAcadApplication, (void**)&Application));
        Application_->Release();

        VARIANT_BOOL Visible;
        THROW_IF_FAILED(Application->get_Visible(&Visible));
        if (!Visible)
        {
            THROW_IF_FAILED(Application->Quit());
            throw std::runtime_error("Visible == false");
        }
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::string() + "no Application (" + e.what() + ")");
    }

    IAcadDocument* ActiveDocument;
    try
    {
        THROW_IF_FAILED(Application->get_ActiveDocument(&ActiveDocument));
        if (!ActiveDocument) throw std::runtime_error("ActiveDocument == 0");
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::string() + "no ActiveDocument (" + e.what() + ")");
    }

    IAcadSelectionSet* ActiveSelectionSet;
    try
    {
        THROW_IF_FAILED(ActiveDocument->get_ActiveSelectionSet(&ActiveSelectionSet));
        if (!ActiveSelectionSet) throw std::runtime_error("ActiveSelectionSet == 0");
        THROW_IF_FAILED(ActiveSelectionSet->Clear());
        THROW_IF_FAILED(ActiveSelectionSet->SelectOnScreen());
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::string() + "selection error (" + e.what() + ")");
    }

    DWORD t = GetTickCount();

    long Count;
    THROW_IF_FAILED(ActiveSelectionSet->get_Count(&Count));
    int callback_delta = Count / 100; // 1%
    if (!callback_delta) callback_delta = 1;
    for (long i = 0; i < Count; i++)
    {
        if (callback && i % callback_delta == 0)
        {
            callback(i, Count);
        }

        VARIANT index;
        V_VT(&index) = VT_I4;
        V_I4(&index) = i;

        IAcadEntity* Item;
        THROW_IF_FAILED(ActiveSelectionSet->Item(index, &Item));

        BSTR EntityName;
        THROW_IF_FAILED(Item->get_EntityName(&EntityName));

        if (!wcscmp(EntityName, L"AcDbBlockReference"))
        {
            IAcadBlockReference* block;
            THROW_IF_FAILED(Item->QueryInterface(IID_IAcadBlockReference, (void**)&block));

            BSTR EffectiveName;
            THROW_IF_FAILED(block->get_EffectiveName(&EffectiveName));

            names->push_back(EffectiveName);

            if (handles)
            {
                BSTR Handle;
                THROW_IF_FAILED(block->get_Handle(&Handle));

                handles->push_back(Handle);
            }

            long lower_bound, upper_bound;

            if (points)
            {
                std::vector<double> point;

                VARIANT InsertionPoint;
                THROW_IF_FAILED(block->get_InsertionPoint(&InsertionPoint));

                THROW_IF_FAILED(SafeArrayGetLBound(V_ARRAY(&InsertionPoint), 1, &lower_bound));
                THROW_IF_FAILED(SafeArrayGetUBound(V_ARRAY(&InsertionPoint), 1, &upper_bound));

                for (long j = lower_bound; j <= upper_bound; j++)
                {
                    double coord;
                    THROW_IF_FAILED(SafeArrayGetElement(V_ARRAY(&InsertionPoint), &j, &coord));
                    point.push_back(coord);
                }

                THROW_IF_FAILED(VariantClear(&InsertionPoint));

                points->push_back(std::move(point));
            }

            // attrs

            std::vector<std::pair<BSTR, BSTR>> attrs;

            VARIANT Attributes;
            THROW_IF_FAILED(block->GetAttributes(&Attributes));

            THROW_IF_FAILED(SafeArrayGetLBound(V_ARRAY(&Attributes), 1, &lower_bound));
            THROW_IF_FAILED(SafeArrayGetUBound(V_ARRAY(&Attributes), 1, &upper_bound));

            for (long j = lower_bound; j <= upper_bound; j++)
            {
                IDispatch* Attribute_;
                THROW_IF_FAILED(SafeArrayGetElement(V_ARRAY(&Attributes), &j, &Attribute_));

                IAcadAttributeReference* Attribute;
                THROW_IF_FAILED(Attribute_->QueryInterface(IID_IAcadAttributeReference, (void**)&Attribute));

                BSTR TagString;
                THROW_IF_FAILED(Attribute->get_TagString(&TagString));

                BSTR TextString;
                THROW_IF_FAILED(Attribute->get_TextString(&TextString));

                attrs.push_back(std::move(std::pair<BSTR, BSTR>(TagString, TextString)));

                Attribute->Release();
            }

            THROW_IF_FAILED(VariantClear(&Attributes));

            attrs_list->push_back(std::move(attrs));

            // props

            std::vector<std::pair<BSTR, VARIANT>> props;

            VARIANT Properties;
            THROW_IF_FAILED(block->GetDynamicBlockProperties(&Properties));

            THROW_IF_FAILED(SafeArrayGetLBound(V_ARRAY(&Properties), 1, &lower_bound));
            THROW_IF_FAILED(SafeArrayGetUBound(V_ARRAY(&Properties), 1, &upper_bound));

            for (long j = lower_bound; j <= upper_bound; j++)
            {
                IDispatch* Property_;
                THROW_IF_FAILED(SafeArrayGetElement(V_ARRAY(&Properties), &j, &Property_));

                IAcadDynamicBlockReferenceProperty* Property;
                THROW_IF_FAILED(Property_->QueryInterface(IID_IAcadDynamicBlockReferenceProperty, (void**)&Property));

                BSTR PropertyName;
                THROW_IF_FAILED(Property->get_PropertyName(&PropertyName));

                VARIANT Value;
                THROW_IF_FAILED(Property->get_Value(&Value));

                props.push_back(std::move(std::pair<BSTR, VARIANT>(PropertyName, Value)));

                Property->Release();
            }

            THROW_IF_FAILED(VariantClear(&Properties));

            props_list->push_back(std::move(props));

            block->Release();
        }

        SysFreeString(EntityName);

        Item->Release();
    }
    if (callback)
    {
        callback(Count, Count);
    }

    long dt = GetTickCount() - t;

    Application->Release();
    ActiveDocument->Release();
    ActiveSelectionSet->Release();

    return dt;
#ifdef AE_EXPORT
    } // try
    catch (...)
    {
        return -1;
    }
#endif
}

#else

static void com_req(VARIANT* result, IDispatch* p, WORD flags, LPCOLESTR name, DISPID* dispID, VARIANT* args, UINT n)
{
    DISPID dispID_ = DISPID_UNKNOWN;
    if (!dispID)
    {
        dispID = &dispID_;
    }
    if (*dispID == DISPID_UNKNOWN)
    {
        LPOLESTR names = LPOLESTR(name);
        THROW_IF_FAILED_EX(p->GetIDsOfNames(IID_NULL, &names, 1, LOCALE_SYSTEM_DEFAULT, dispID), name);
    }

    DISPPARAMS params = {args, 0, n, 0};

    VARIANT result_;
    if (!result)
    {
        result = &result_;
    }

    THROW_IF_FAILED_EX(p->Invoke(*dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, flags, &params, result, 0, 0), name);
}

AE_EXTERN_C long AE_DECL_SPEC select_on_screen
(
    std::vector<BSTR>* const names,
    std::vector<std::vector<std::pair<BSTR, BSTR>>>* const attrs_list,
    std::vector<std::vector<std::pair<BSTR, VARIANT>>>* const props_list,
    std::vector<BSTR>* const handles,
    std::function<void(int processed, int total)> callback,
    std::vector<std::vector<double>>* const points
)
{
#ifdef AE_EXPORT
    try
    {
#endif
    init();

    CLSID clsid;
    {
        HRESULT hr = CLSIDFromProgID(L"AutoCAD.Application", &clsid);
        if (FAILED(hr)) THROW_HR(hr);
    }

    VARIANT args[7];

    IDispatch* Application;
    try
    {
        IUnknown* Application_;
        THROW_IF_FAILED(GetActiveObject(clsid, 0, &Application_));
        THROW_IF_FAILED(Application_->QueryInterface(IID_IDispatch, (void**)&Application));
        Application_->Release();

        VARIANT Visible;
        com_req(&Visible, Application, DISPATCH_PROPERTYGET, L"Visible", 0, args, 0);
        if (!V_BOOL(&Visible))
        {
            com_req(0, Application, DISPATCH_METHOD, L"Quit", 0, args, 0);
            throw std::runtime_error("Visible == false");
        }
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::string() + "no Application (" + e.what() + ")");
    }

    IDispatch* ActiveDocument;
    try
    {
        VARIANT ActiveDocument_;
        com_req(&ActiveDocument_, Application, DISPATCH_PROPERTYGET, L"ActiveDocument", 0, args, 0);
        ActiveDocument = V_DISPATCH(&ActiveDocument_);
        ActiveDocument->AddRef();
        VariantClear(&ActiveDocument_);
        if (!ActiveDocument) throw std::runtime_error("ActiveDocument == 0");
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::string() + "no ActiveDocument (" + e.what() + ")");
    }

    IDispatch* ActiveSelectionSet;
    try
    {
        VARIANT ActiveSelectionSet_;
        com_req(&ActiveSelectionSet_, ActiveDocument, DISPATCH_PROPERTYGET, L"ActiveSelectionSet", 0, args, 0);
        ActiveSelectionSet = V_DISPATCH(&ActiveSelectionSet_);
        ActiveSelectionSet->AddRef();
        VariantClear(&ActiveSelectionSet_);
        if (!ActiveSelectionSet) throw std::runtime_error("ActiveSelectionSet == 0");
        com_req(0, ActiveSelectionSet, DISPATCH_METHOD, L"Clear", 0, args, 0);
        com_req(0, ActiveSelectionSet, DISPATCH_METHOD, L"SelectOnScreen", 0, args, 0);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::string() + "selection error (" + e.what() + ")");
    }

    DWORD t = GetTickCount();

    long Count;
    VARIANT Count_;
    com_req(&Count_, ActiveSelectionSet, DISPATCH_PROPERTYGET, L"Count", 0, args, 0);
    Count = V_I4(&Count_);
    DISPID ItemDispID = DISPID_UNKNOWN;
    DISPID EntityNameDispID = DISPID_UNKNOWN;
    DISPID EffectiveNameDispID = DISPID_UNKNOWN;
    DISPID HandleDispID = DISPID_UNKNOWN;
    DISPID InsertionPointDispID = DISPID_UNKNOWN;
    DISPID GetAttributesDispID = DISPID_UNKNOWN;
    DISPID TagStringDispID = DISPID_UNKNOWN;
    DISPID TextStringDispID = DISPID_UNKNOWN;
    DISPID GetDynamicBlockPropertiesDispID = DISPID_UNKNOWN;
    DISPID PropertyNameDispID = DISPID_UNKNOWN;
    DISPID ValueDispID = DISPID_UNKNOWN;
    int callback_delta = Count / 100; // 1%
    if (!callback_delta) callback_delta = 1;
    for (long i = 0; i < Count; i++)
    {
        if (callback && i % callback_delta == 0)
        {
            callback(i, Count);
        }

        V_VT(&args[0]) = VT_I4;
        V_I4(&args[0]) = i;

        IDispatch* Item;
        VARIANT Item_;
        com_req(&Item_, ActiveSelectionSet, DISPATCH_METHOD, L"Item", &ItemDispID, args, 1);
        Item = V_DISPATCH(&Item_);
        Item->AddRef();
        VariantClear(&Item_);

        VARIANT EntityName;
        com_req(&EntityName, Item, DISPATCH_PROPERTYGET, L"EntityName", &EntityNameDispID, args, 0);

        if (!wcscmp(V_BSTR(&EntityName), L"AcDbBlockReference"))
        {
            VARIANT EffectiveName;
            com_req(&EffectiveName, Item, DISPATCH_PROPERTYGET, L"EffectiveName", &EffectiveNameDispID, args, 0);

            names->push_back(V_BSTR(&EffectiveName));

            if (handles)
            {
                VARIANT Handle;
                com_req(&Handle, Item, DISPATCH_PROPERTYGET, L"Handle", &HandleDispID, args, 0);

                handles->push_back(V_BSTR(&Handle));
            }

            long lower_bound, upper_bound;

            if (points)
            {
                std::vector<double> point;

                VARIANT InsertionPoint;
                com_req(&InsertionPoint, Item, DISPATCH_PROPERTYGET, L"InsertionPoint", &InsertionPointDispID, args, 0);

                THROW_IF_FAILED(SafeArrayGetLBound(V_ARRAY(&InsertionPoint), 1, &lower_bound));
                THROW_IF_FAILED(SafeArrayGetUBound(V_ARRAY(&InsertionPoint), 1, &upper_bound));

                for (long j = lower_bound; j <= upper_bound; j++)
                {
                    double coord;
                    THROW_IF_FAILED(SafeArrayGetElement(V_ARRAY(&InsertionPoint), &j, &coord));
                    point.push_back(coord);
                }

                THROW_IF_FAILED(VariantClear(&InsertionPoint));

                points->push_back(std::move(point));
            }

            // attrs

            std::vector<std::pair<BSTR, BSTR>> attrs;

            VARIANT Attributes;
            com_req(&Attributes, Item, DISPATCH_METHOD, L"GetAttributes", &GetAttributesDispID, args, 0);

            THROW_IF_FAILED(SafeArrayGetLBound(V_ARRAY(&Attributes), 1, &lower_bound));
            THROW_IF_FAILED(SafeArrayGetUBound(V_ARRAY(&Attributes), 1, &upper_bound));

            for (long j = lower_bound; j <= upper_bound; j++)
            {
                IDispatch* Attribute;
                THROW_IF_FAILED(SafeArrayGetElement(V_ARRAY(&Attributes), &j, &Attribute));

                VARIANT TagString;
                com_req(&TagString, Attribute, DISPATCH_PROPERTYGET, L"TagString", &TagStringDispID, args, 0);

                VARIANT TextString;
                com_req(&TextString, Attribute, DISPATCH_PROPERTYGET, L"TextString", &TextStringDispID, args, 0);

                attrs.push_back(std::move(std::pair<BSTR, BSTR>(V_BSTR(&TagString), V_BSTR(&TextString))));

                Attribute->Release();
            }

            THROW_IF_FAILED(VariantClear(&Attributes));

            attrs_list->push_back(std::move(attrs));

            // props

            std::vector<std::pair<BSTR, VARIANT>> props;

            VARIANT Properties;
            com_req(&Properties, Item, DISPATCH_METHOD, L"GetDynamicBlockProperties", &GetDynamicBlockPropertiesDispID, args, 0);

            THROW_IF_FAILED(SafeArrayGetLBound(V_ARRAY(&Properties), 1, &lower_bound));
            THROW_IF_FAILED(SafeArrayGetUBound(V_ARRAY(&Properties), 1, &upper_bound));

            for (long i = lower_bound; i <= upper_bound; i++)
            {
                IDispatch* Property;
                THROW_IF_FAILED(SafeArrayGetElement(V_ARRAY(&Properties), &i, &Property));

                VARIANT PropertyName;
                com_req(&PropertyName, Property, DISPATCH_PROPERTYGET, L"PropertyName", &PropertyNameDispID, args, 0);

                VARIANT Value;
                com_req(&Value, Property, DISPATCH_PROPERTYGET, L"Value", &ValueDispID, args, 0);

                props.push_back(std::move(std::pair<BSTR, VARIANT>(V_BSTR(&PropertyName), Value)));

                Property->Release();
            }

            THROW_IF_FAILED(VariantClear(&Properties));

            props_list->push_back(std::move(props));
        }

        VariantClear(&EntityName);

        Item->Release();
    }
    if (callback)
    {
        callback(Count, Count);
    }

    long dt = GetTickCount() - t;

    Application->Release();
    ActiveDocument->Release();
    ActiveSelectionSet->Release();

    return dt;
#ifdef AE_EXPORT
    } // try
    catch (...)
    {
        return -1;
    }
#endif
}

#endif
