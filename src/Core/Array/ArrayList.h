/**
 * Copyright 2021 Xiaowei He
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <list>
#include <iostream>
#include "STL/List.h"

namespace dyno
{
	template<class ElementType, DeviceType deviceType> class ArrayList;

	template<class ElementType>
	class ArrayList<ElementType, DeviceType::CPU>
	{
	public:
		ArrayList() {};
		~ArrayList()
		{
			m_index.clear();
			m_elements.clear();
			m_lists.clear();
		}

		bool resize(size_t num);

		inline const size_t size() const { return m_lists.size(); }
		size_t elementSize();

		inline List<ElementType>& operator [] (unsigned int id)
		{
			return m_lists[id];
		}

		inline List<ElementType> operator [] (unsigned int id) const
		{
			return m_lists[id];
		}

		inline bool isCPU() const { return true; }
		inline bool isGPU() const { return false; }
		inline bool isEmpty() const { return m_lists.empty(); }

		void clear();

		void assign(const ArrayList<ElementType, DeviceType::CPU>& src);
		void assign(const ArrayList<ElementType, DeviceType::GPU>& src);

		friend std::ostream& operator<<(std::ostream &out, const ArrayList<ElementType, DeviceType::CPU>& aList)
		{
			out << std::endl;
			for (int i = 0; i < aList.size(); i++)
			{
				List<ElementType>& lst = aList[i];
				out << "List " << i << " (" << lst.size() << "):";
				for (auto it = lst.begin(); it != lst.end(); it++)
				{
					std::cout << " " << *it;
				}
				out << std::endl;
			}
			return out;
		}

		const CArray<int>& index() const { return m_index; }
		const CArray<ElementType> elements() const { return m_elements; }
		const CArray<List<ElementType>> lists() const { return m_lists; }

	private:
		CArray<int> m_index;
		CArray<ElementType> m_elements;

		CArray<List<ElementType>> m_lists;
	};

	template<class ElementType>
	class ArrayList<ElementType, DeviceType::GPU>
	{
	public:
		ArrayList()
		{
		};

		/*!
		*	\brief	Do not release memory here, call clear() explicitly.
		*/
		~ArrayList() {};

		/**
		 * @brief Pre-allocate GPU space for
		 *
		 * @param counts
		 * @return true
		 * @return false
		 */
		bool resize(const GArray<int> counts);
		bool resize(const size_t arraySize, const size_t eleSize);

		DYN_FUNC inline int size() const { return m_lists.size(); }
		DYN_FUNC inline int elementSize() const { return m_elements.size(); }

		GPU_FUNC inline List<ElementType>& operator [] (unsigned int id)
		{
			return m_lists[id];
		}

		GPU_FUNC inline List<ElementType> operator [] (unsigned int id) const
		{
			return m_lists[id];
		}

		DYN_FUNC inline bool isCPU() const { return false; }
		DYN_FUNC inline bool isGPU() const { return true; }
		DYN_FUNC inline bool isEmpty() const { return m_index.size() == 0; }

		void release();

		void assign(const ArrayList<ElementType, DeviceType::GPU>& src);
		void assign(const ArrayList<ElementType, DeviceType::CPU>& src);
		void assign(const std::vector<std::vector<ElementType>>& src);

		friend std::ostream& operator<<(std::ostream &out, const ArrayList<ElementType, DeviceType::GPU>& aList)
		{
			ArrayList<ElementType, DeviceType::CPU> hList;
			hList.assign(aList);
			out << hList;

			return out;
		}

		const GArray<int>& index() const { return m_index; }
		const GArray<ElementType> elements() const { return m_elements; }
		const GArray<List<ElementType>> lists() const { return m_lists; }

	private:
		GArray<int> m_index;
		GArray<ElementType> m_elements;

		GArray<List<ElementType>> m_lists;
	};

	template<typename T>
	using GArrayList = ArrayList<T, DeviceType::GPU>;

	template<typename T>
	using CArrayList = ArrayList<T, DeviceType::CPU>;
}

#include "ArrayList.inl"
