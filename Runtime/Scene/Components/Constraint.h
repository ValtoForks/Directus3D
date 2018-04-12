/*
Copyright(c) 2016-2018 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES =====================
#include "IComponent.h"
#include "../../Math/Vector3.h"
#include "../../Math/Vector2.h"
#include "../../Math/Quaternion.h"
//================================

class btTypedConstraint;

namespace Directus
{
	class RigidBody;
	class GameObject;

	enum ConstraintType
	{
		ConstraintType_Point,
		ConstraintType_Hinge,
		ConstraintType_Slider,
		ConstraintType_ConeTwist
	};

	class ENGINE_CLASS Constraint : public IComponent
	{
	public:
		Constraint(Context* context, GameObject* gameObject, Transform* transform);
		~Constraint();

		//= COMPONENT ================================
		void OnInitialize() override;
		void OnStart() override;
		void OnStop() override;
		void OnRemove() override;
		void OnUpdate() override;
		void Serialize(FileStream* stream) override;
		void Deserialize(FileStream* stream) override;
		//============================================

		void SetConstraintType(ConstraintType type);

		const Math::Vector2& GetHighLimit() { return m_highLimit; }
		// Set high limit. Interpretation is constraint type specific.
		void SetHighLimit(const Math::Vector2& limit);

		const Math::Vector2& GetLowLimit() { return m_lowLimit; }
		// Set low limit. Interpretation is constraint type specific.
		void SetLowLimit(const Math::Vector2& limit);

		const Math::Vector3& GetPosition() { return m_position; }
		// Set constraint position relative to own body.
    	void SetPosition(const Math::Vector3& position);

		const Math::Quaternion& GetRotation() { return m_rotation; }
		// Set constraint rotation relative to own body.
		void SetRotation(const Math::Quaternion& rotation);

	private:
		void ConstructConstraint();
		void ApplyLimits();
		void ApplyFrames();
		void ReleaseConstraint();

		std::unique_ptr<btTypedConstraint> m_constraint;
		std::weak_ptr<RigidBody> m_bodyOwn;
		std::weak_ptr<RigidBody> m_bodyOther;
		Math::Vector3 m_position;
		Math::Quaternion m_rotation;
		Math::Vector2 m_highLimit;
		Math::Vector2 m_lowLimit;
		float m_errorReduction;
		float m_constraintForceMixing;
		bool m_isDirty;
		bool m_enabledEffective;
		bool m_collisionWithLinkedBody;
		ConstraintType m_type;
	};
}