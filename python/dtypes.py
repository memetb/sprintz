#!/usr/bin/env python

import numpy as np
import pandas as pd

FLOAT_TYPES = [np.float16, np.float32, np.float64]

UNSIGNED_INT_TYPES = [np.uint8, np.uint16, np.uint32, np.uint64]
SIGNED_INT_TYPES = [np.int8, np.int16, np.int32, np.int64]
INT_TYPES = UNSIGNED_INT_TYPES + SIGNED_INT_TYPES

NULLABLE_SIGNED_INT_TYPES = [
    pd.Int8Dtype, pd.Int16Dtype, pd.Int32Dtype, pd.Int64Dtype]
NULLABLE_UNSIGNED_INT_TYPES = [
    pd.UInt8Dtype, pd.UInt16Dtype, pd.UInt32Dtype, pd.UInt64Dtype]
NULLABLE_INT_TYPES = NULLABLE_UNSIGNED_INT_TYPES + NULLABLE_SIGNED_INT_TYPES

NULLABLE_TO_NONNULLABLE_INT_DTYPE = dict(zip(NULLABLE_INT_TYPES, INT_TYPES))
NONNULLABLE_TO_NULLABLE_INT_DTYPE = dict(zip(INT_TYPES, NULLABLE_INT_TYPES))

NUMERIC_DTYPES = INT_TYPES + NULLABLE_INT_TYPES + FLOAT_TYPES

# NOTE: np.bool is alias of python bool, while np.bool_ is custom a numpy type
BOOLEAN_DTYPES = [np.bool, np.bool_, pd.BooleanDtype]


def _canonicalize(dtype):
    try:
        return dtype.type
    except AttributeError:
        return dtype


def nullable_equivalent(dtype):
    # TODO support nullable strings and other pandas dtypes
    dtype = _canonicalize(dtype)
    if dtype in FLOAT_TYPES:
        return dtype
    return NULLABLE_TO_NONNULLABLE_INT_DTYPE[dtype]


def is_complex(dtype):
    return pd.api.types.is_complex_dtype(dtype)


def is_float(dtype):
    return pd.api.types.is_float_dtype(dtype)
    # return _canonicalize(dtype) in FLOAT_TYPES


def is_numeric(dtype):
    return pd.api.types.is_float_dtype(dtype)


def is_boolean(dtype):
    return _canonicalize(dtype) in BOOLEAN_DTYPES


def is_int(dtype):
    return pd.api.types.is_integer_dtype(dtype)


def is_signed_int(dtype):
    return pd.api.types.is_signed_integer_dtype(dtype)


def is_unsigned_int(dtype):
    return pd.api.types.is_unsigned_integer_dtype(dtype)


def is_object(dtype):
    return pd.api.types.is_object_dtype(dtype)


def is_pandas_extension_type(dtype):
    return api.types.is_extension_array_dtype(dtype)


def is_fixed_size(dtype):
    if is_object(dtype):
        return False
    return True

    # # try:
    # #     ar = np.array([], dtype=dtype)
    # #     _ = ar.itemsize
    # # # dtype = _canonicalize(dtype)

    # if is_float(dtype):
    #     return True
    # if is_complex(dtype):
    #     return True


    # # XXX string and byte dtypes can be fixed size
    # return not np.is_nullable(dtype)


def is_nullable(dtype):
    dtype = _canonicalize(dtype)
    if dtype in NULLABLE_INT_TYPES:
        return True
    if dtype in FLOAT_TYPES:
        return True

    # XXX include other nullable dtypes

    return False


# used for codec type whitelists/blacklists
# note that typelist can contain types, unary functions of types, and
# keywords like "numeric"
# note that this is an OR of whether it matches each one, not AND
def dtype_in_list(dtype, typelist):
    dtype = _canonicalize(dtype)
    typelist = [_canonicalize(dtype_or_func) for dtype_or_func in typelist]

    if dtype in typelist:
        return True  # easy case; dtype is in typelist

    for typ_or_func in typelist:
        if callable(type_or_func):
            f = type_or_func
            if f(dtype):
                return True
        else:  # not callable
            typ = type_or_func
            if typ == 'numeric' and is_numeric(dtype):
                return True
            if typ == 'anyint' and is_int(dtype):
                return True
            if typ == 'signedint' and is_signed_int(dtype):
                return True
            if typ == 'unsignedint' and is_unsigned_int(dtype):
                return True
            if typ == 'complex' and is_complex(dtype):
                return True
            if typ == 'anyfloat' and is_float(dtype):
                return True
            if typ == 'anybool' and is_boolean(dtype):
                return True
            if typ == 'nullable' and is_nullable(dtype):
                return True
            if typ == 'nonnullable' and not is_nullable(dtype):
                return True
            if typ == np.object and is_object(dtype):
                return True

    return False



        # f = to_func()


