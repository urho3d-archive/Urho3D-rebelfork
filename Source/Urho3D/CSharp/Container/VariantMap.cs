﻿//
// Copyright (c) 2018 Rokas Kupstys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security;
using System.Threading;
using Urho3D.CSharp;

namespace Urho3D
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct HashIteratorBase
    {
        public IntPtr ptr_;
    }

    public class VariantMap : NativeObject, IDictionary<StringHash, Variant>
    {
        #region Interop
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Urho3D_HashMap_HashMap();
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint Urho3D_HashMap_StringHash_Variant_GetKey(HashIteratorBase it);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Urho3D_HashMap_StringHash_Variant_GetValue(HashIteratorBase it);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void Urho3D_HashMap_StringHash_Variant_Add(IntPtr map, uint key, IntPtr value);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool Urho3D_HashMap_StringHash_Variant_Remove(IntPtr map, uint key);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool Urho3D_HashMap_StringHash_Variant_First(IntPtr map, out HashIteratorBase it);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool Urho3D_HashMap_StringHash_Variant_Next(IntPtr map, ref HashIteratorBase it);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern bool Urho3D_HashMap_StringHash_Variant_Contains(IntPtr map, uint key);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Urho3D_HashMap_StringHash_Variant_TryGet(IntPtr map, uint key);
        [SuppressUnmanagedCodeSecurity]
        [DllImport(Config.NativeLibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void Urho3D_HashMap_StringHash_Variant_destructor(IntPtr map);
        #endregion

        class Enumerator : IEnumerator<KeyValuePair<StringHash, Variant>>
        {
            private readonly VariantMap _hashMap;
            private HashIteratorBase _iterator;
            private bool _isFirst = true;

            public Enumerator(VariantMap hashMap)
            {
                _hashMap = hashMap;
                Reset();
            }

            public bool MoveNext()
            {
                if (_isFirst)
                {
                    _isFirst = false;
                    return Urho3D_HashMap_StringHash_Variant_First(_hashMap.NativeInstance, out _iterator);
                }
                return Urho3D_HashMap_StringHash_Variant_Next(_hashMap.NativeInstance, ref _iterator);
            }

            public void Reset()
            {
                _isFirst = true;
            }

            public KeyValuePair<StringHash, Variant> Current
            {
                get
                {
                    var rawKey = Urho3D_HashMap_StringHash_Variant_GetKey(_iterator);
                    var rawVal = Urho3D_HashMap_StringHash_Variant_GetValue(_iterator);
                    var key = StringHash.GetManagedInstance(rawKey);
                    var value = Variant.GetManagedInstance(rawVal);
                    return new KeyValuePair<StringHash, Variant>(key, value);
                }
            }

            object IEnumerator.Current => Current;

            public void Dispose()
            {
            }
        }

        public VariantMap()
        {
            SetupInstance(Urho3D_HashMap_HashMap());
        }

        internal VariantMap(IntPtr instance, NativeObjectFlags flags=NativeObjectFlags.None)
        {
            SetupInstance(instance, flags);
        }

        public IEnumerator<KeyValuePair<StringHash, Variant>> GetEnumerator()
        {
            return new Enumerator(this);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void Add(KeyValuePair<StringHash, Variant> item)
        {
            Add(item.Key, item.Value);
        }

        public void Clear()
        {
        }

        public bool Contains(KeyValuePair<StringHash, Variant> item)
        {
            return this.Any(kv => kv.Key.Equals(item.Key) && kv.Value.Equals(item.Value));
        }

        public void CopyTo(KeyValuePair<StringHash, Variant>[] array, int arrayIndex)
        {
            foreach (var kv in this)
                array[arrayIndex++] = kv;
        }

        public bool Remove(KeyValuePair<StringHash, Variant> item)
        {
            return Contains(item) && Remove(item.Key);
        }

        public int Count
        {
            get
            {
                int count = 0;
                using (var enumerator = GetEnumerator())
                    for (; enumerator.MoveNext(); count++) { }
                return count;
            }
        }

        public bool IsReadOnly => false;
        public void Add(StringHash key, Variant value)
        {
            var pInvokeKey = StringHash.GetNativeInstance(key);
            var pInvokeValue = Variant.GetNativeInstance(value);
            Urho3D_HashMap_StringHash_Variant_Add(NativeInstance, pInvokeKey, pInvokeValue);
        }

        public bool ContainsKey(StringHash key)
        {
            return Urho3D_HashMap_StringHash_Variant_Contains(NativeInstance, key.Hash);
        }

        public bool Remove(StringHash key)
        {
            return Urho3D_HashMap_StringHash_Variant_Remove(NativeInstance, key.Hash);
        }

        public bool TryGetValue(StringHash key, out Variant value)
        {
            var instance = Urho3D_HashMap_StringHash_Variant_TryGet(NativeInstance, key.Hash);
            if (instance == IntPtr.Zero)
            {
                value = null;
                return false;
            }
            value = Variant.GetManagedInstance(instance);
            return true;
        }

        public Variant this[StringHash key]
        {
            get
            {
                Variant value;
                if (TryGetValue(key, out value))
                    return value;
                throw new KeyNotFoundException();
            }
            set { Add(key, value); }
        }

        public ICollection<StringHash> Keys => this.Select(kv => kv.Key).Distinct().ToList();
        public ICollection<Variant> Values => this.Select(kv => kv.Value).Distinct().ToList();

        internal static VariantMap GetManagedInstance(IntPtr source, NativeObjectFlags flags=NativeObjectFlags.None)
        {
            return InstanceCache.GetOrAdd(source, ptr => new VariantMap(ptr, flags));
        }

        internal static IntPtr GetNativeInstance(VariantMap source)
        {
            if (source == null)
            {
                return IntPtr.Zero;
            }
            return source.NativeInstance;
        }

        protected override void Dispose(bool disposing)
        {
            InstanceCache.Remove(NativeInstance);
            if (!NonOwningReference)
                Urho3D_HashMap_StringHash_Variant_destructor(NativeInstance);
        }
    }
}
